/************************************************************************/
/*   noCheat Kernel-Mode Driver                                         */
/*      This driver is used by the noCheat system to monitor            */
/*      processes being created and to monitor which DLLs or            */
/*      modules they load.                                              */
/*                                                                      */
/*                                                                      */
/*      This kernel-mode driver should not be altered passed            */
/*      its intended purpose, and is to be tested thoroughly            */
/*      before being released to the public.                            */
/************************************************************************/

#include <ntddk.h>

#define _NC_DRIVER
#include "driverdefines.h"

// Debug?
//	Comment out to set to release mode
//	(**THIS MUST BE DONE MANUALLY SINCE VS
//	DOES NOT SIGNAL TO THE BUILD ENVIRONMENTS!**)
//#define DEBUG

// Setup logging
#ifdef DEBUG
#define NCLOG(a) DbgPrint("[nC]%s\n",a)
#else
#define NCLOG(a)
#endif

// Our nC device/link constants
const WCHAR devicename[] = L"\\Device\\noCheat";
const WCHAR devicelink[] = L"\\DosDevices\\NOCHEAT";

/*
 * Driver vars setup
 */
NTSTATUS cbRegistered;
struct NC_EVENT_SETTINGS ncImageLoadEventSettings;
KEVENT event;

/*
 * Waits for the service to finish writing to the buffer
 */
void waitForService()
{
	struct NC_IL_HEAD* h = (struct NC_IL_HEAD*)ncImageLoadEventSettings.buff;
	while(h->swrite == 1);
}

/*
 * Returns the lowest of the two
 */
unsigned char minChar(unsigned char a, unsigned char b)
{
	// Return min
	return (a < b ? a : b);
}

/*
 * Image Load Callback
 */
VOID ImageLoadCallback(
							  __in_opt PUNICODE_STRING FullImageName,
							  __in HANDLE  ProcessId,
							  __in PIMAGE_INFO  ImageInfo
							  )
{
	ANSI_STRING strr;
	void* pnt;
	struct NC_IL_INFO ncInf;
	struct NC_IL_HEAD* ncILHead;

	// Check to see nCS has connected and return if not
	if(ncImageLoadEventSettings.buff == NULL) return;

	// Setup head
	ncILHead = (struct NC_IL_HEAD*)ncImageLoadEventSettings.buff;

	// Check for overflow
	if(ncILHead->count >= ncImageLoadEventSettings.maxEvents)
	{
		NCLOG("Reached maximum events on stack!");
		return;
	}

	// Define/populate our condensed struct to be passed
	//	back to the user-land noCheat service
	ncInf.hwndProcessId = ProcessId;
	memcpy(&ncInf.pinfImageInfo, ImageInfo, sizeof(IMAGE_INFO));
	memset(&ncInf.puszFullImageName, 0, 260);

	// Convert to ansi string, copy, and free
	RtlUnicodeStringToAnsiString(&strr, FullImageName, 1);
	memcpy(&ncInf.puszFullImageName, strr.Buffer, strr.Length);
	RtlFreeAnsiString(&strr);

	// Calculate offset
	pnt = (void*)((int)ncImageLoadEventSettings.buff + sizeof(struct NC_IL_HEAD) + ((int)ncILHead->count * sizeof(struct NC_IL_INFO)));

	// Wait for service to complete
	waitForService();

	// Mark that we're writing
	ncILHead->dwrite = 1;

	// Copy~!
	memcpy(pnt, &ncInf, sizeof(struct NC_IL_INFO));

	// Increment stack count
	ncILHead->count = ncILHead->count + 1;

	// Mark that we're done writing
	ncILHead->dwrite = 0;

	// Debug
#ifdef DEBUG
	DbgPrint("[nC]Image(%d): %wZ\n", ProcessId, FullImageName);
#endif
}

NTSTATUS DrvDispatch(IN PDEVICE_OBJECT device, IN PIRP Irp)
{
	// Setup vars
	unsigned char* buff;
	PIO_STACK_LOCATION pLoc;
	struct NC_CONNECT_INFO* ncCInfo;

	// Debug
	NCLOG("Dispatching driver link");

	// Get our current stack location
	pLoc = IoGetCurrentIrpStackLocation(Irp);

	// Check that our control code is correct
	//	This *should* change from time to time to
	//	increase security
	if(pLoc->Parameters.DeviceIoControl.IoControlCode == 1000)
	{		
		// Get the contents
		buff = (UCHAR*)Irp->AssociatedIrp.SystemBuffer;
		
		// Create a pointer to connect information
		ncCInfo = (struct NC_CONNECT_INFO*)buff;

		// Check our security
		if(ncCInfo->secCode == EAT_STOOL)
		{
			// Security code passed! - Debug
			NCLOG("Passed security check");

			// Set up image-loading events
			ncImageLoadEventSettings.buff = (char*)MmMapIoSpace(MmGetPhysicalAddress((void*)ncCInfo->ILBuffAddr), (SIZE_T)ncCInfo->ILBuffLen, 0);
			memset(ncImageLoadEventSettings.buff, 0, ncCInfo->ILBuffLen);
			ncImageLoadEventSettings.buffSize = ncCInfo->ILBuffLen;
			ncImageLoadEventSettings.maxEvents = minChar(255, (char)((ncCInfo->ILBuffLen - sizeof(struct NC_IL_HEAD)) / sizeof(struct NC_IL_INFO)));

#ifdef DEBUG
			// Report sizes
			DbgPrint("[nC]IL Event Length %d | Total Buffered Events %d\n", sizeof(struct NC_IL_INFO), ncImageLoadEventSettings.maxEvents);

			// Check if there is a fall-off and report it
			if(((ncCInfo->ILBuffLen - sizeof(struct NC_IL_HEAD)) % sizeof(struct NC_IL_INFO)) > 0)
				DbgPrint("[nC]ILBuff Falloff! %d bytes can be trimmed!\n", ((ncCInfo->ILBuffLen - sizeof(struct NC_IL_HEAD)) % sizeof(struct NC_IL_INFO)));
#endif

			// Debug
			NCLOG("Ready for reporting!");
		}else{
			// Did not pass security check!
			NCLOG("Did not pass security check; ignoring");
		}
	}

	// Set success status
	Irp->IoStatus.Status = 0;

	// Complete the request
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	// Return success
	return 0;
}

/*
 * Called when a device/file is opened/closed.
 *	This is used to ready-up our IO Link
 */
NTSTATUS DrvCreateClose(IN PDEVICE_OBJECT obj, IN PIRP Irp)
{
	// Debug
	NCLOG("DrvIO Create/Close");

	// Clear our buffer handle
	//	This signals to the driver that there is no longer a
	//	buffer to write to
	ncImageLoadEventSettings.buff = NULL;

	// Null out event values (reset)
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = 0;

	// Complete request
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	// Return success
	return 0;
}

/*
 * Driver unload function
 */
void DrvUnload(IN PDRIVER_OBJECT driver)
{
	UNICODE_STRING devLink;

	// Unmap our output
	MmUnmapIoSpace(ncImageLoadEventSettings.buff, ncImageLoadEventSettings.buffSize);

	// Setup string
	RtlInitUnicodeString(&devLink, devicelink);

	// Delete symlink
	IoDeleteSymbolicLink(&devLink);

	// Delete device
	IoDeleteDevice(driver->DeviceObject);

	// Remove our callback if it had succeeded (this should
	//	always be true)
	if(cbRegistered == STATUS_SUCCESS)
		PsRemoveLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE)&ImageLoadCallback);

	// Output our debug
	NCLOG("Unloaded Driver");
}

/*
 * Main Driver Entry Point
 */
NTSTATUS DriverEntry(IN PDRIVER_OBJECT driver, IN PUNICODE_STRING path)
{
	// Setup vars
	PDEVICE_OBJECT devObj = NULL;
	UNICODE_STRING devLink, devName;
	NTSTATUS ntsDevCreate;

	// Break
	//DbgBreakPoint();

	// Convert our strings
	RtlInitUnicodeString(&devLink, devicelink);
	RtlInitUnicodeString(&devName, devicename);

	// Create device
	ntsDevCreate = IoCreateDevice(driver, 256, &devName, FILE_DEVICE_UNKNOWN, 0, TRUE, &devObj);
	if(ntsDevCreate == STATUS_SUCCESS)
	{
		// Debug
		NCLOG("Successfully created IO Device");

		// Create link
		if(IoCreateSymbolicLink(&devLink, &devName) != STATUS_SUCCESS)
			NCLOG("Could not create symbolic link!");
	}else {
		switch(ntsDevCreate)
		{
		case STATUS_INSUFFICIENT_RESOURCES:
			NCLOG("Could not create device: Insufficient Resources");
			break;
		case STATUS_OBJECT_NAME_EXISTS:
			NCLOG("Could not create device: Object name already exists");
			break;
		case STATUS_OBJECT_NAME_COLLISION:
			NCLOG("Could not create device: Object name collision");
			break;
		default:
			NCLOG("Could not create device: Unknown error");
			break;
		}
	}

	// Set major functions
	driver->MajorFunction[IRP_MJ_CREATE] = DrvCreateClose;
	driver->MajorFunction[IRP_MJ_CLOSE] = DrvCreateClose;
	driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DrvDispatch;

	// Set unload function
	driver->DriverUnload = DrvUnload;

	// Setup synchronization
	KeInitializeEvent(&event, SynchronizationEvent, 1);

	// Store the success of our callback
	cbRegistered = PsSetLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE)&ImageLoadCallback);

	// Check callback success and debug
	if(cbRegistered == STATUS_SUCCESS)
		NCLOG("Successfully Registered Callback");
	else
	{
		NCLOG("Could not load callback!");

		// Return severe error
		return STATUS_SEVERITY_ERROR;
	}

	// Debug that we're starting!
	NCLOG("Starting Driver");

	// Return success!
	return STATUS_SUCCESS;
}
