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

// Debug?
//	Comment out to set to release mode
//	(**THIS MUST BE DONE MANUALLY SINCE VS
//	DOES NOT SIGNAL TO THE BUILD ENVIRONMENTS!**)
#define DEBUG

// Setup logging
#ifdef DEBUG
#define NCLOG(a) DbgPrint("[nC]%s",a)
#else
#define NCLOG(a)
#endif

// Our nC device/link constants
const WCHAR devicename[] = L"\\Device\\noCheat";
const WCHAR devicelink[] = L"\\DosDevices\\NOCHEAT";

#define EAT_STOOL 0xEA757001

/*
 * Connection info sent from the service
 */
struct NC_CONNECT_INFO
{
	unsigned long secCode; // The security code - currently EAT_STOOL
	unsigned long ILBuffAddr;	// The address to map to for image-loading events
	unsigned long ILBuffLen; // The buffer length (in bytes) for image-loading events
};

/*
 * Holds information about the buffer totals
 *	and event sizes and such
 */
struct NC_EVENT_SETTINGS
{
	unsigned long buffSize;
	unsigned long maxEvents;
	unsigned char* buff;
};

/*
 * Our condensed image load struct
 *	that is passed back to the user-land service
 */
struct NC_IL_INFO
{
	char puszFullImageName[260]; // MAX_PATH, which is not available in the DDK
	HANDLE hwndProcessId;
	PIMAGE_INFO pinfImageInfo;
};

/*
 * Driver vars setup
 */
NTSTATUS cbRegistered;
struct NC_EVENT_SETTINGS ncImageLoadEventSettings;
KEVENT event;

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

	// Define/populate our condensed struct to be passed
	//	back to the user-land noCheat service
	struct NC_IL_INFO ncInf;
	ncInf.hwndProcessId = ProcessId;
	ncInf.pinfImageInfo = ImageInfo;
	
	// Memset
	memset(&ncInf.puszFullImageName, 0, 260);

	// Convert to ansi string
	RtlUnicodeStringToAnsiString(&strr, FullImageName, 1);

	// Memcpy
	memcpy(&ncInf.puszFullImageName, strr.Buffer, strr.Length);

	// Free
	RtlFreeAnsiString(&strr);

	// Wait for access to buffer
	KeWaitForSingleObject(&event,Executive,KernelMode,0,0);

	// Check for overflow
	if(ncImageLoadEventSettings.buff[0] >= ncImageLoadEventSettings.maxEvents)
	{
		NCLOG("Reached maximum events on stack!");
		return;
	}

	// Increment stack count
	ncImageLoadEventSettings.buff[0] = ncImageLoadEventSettings.buff[0] + 1;

	// Calculate offset
	pnt = (int)ncImageLoadEventSettings.buff + ((int)ncImageLoadEventSettings.buff[0] * sizeof(struct NC_IL_INFO)) + 1;

	// Copy~!
	memcpy(pnt, &ncInf, sizeof(struct NC_IL_INFO));

	// Debug
#ifdef DEBUG
	DbgPrint("[nC]Image(%d): %wZ", ProcessId, FullImageName);
#endif
}

NTSTATUS DrvDispatch(IN PDEVICE_OBJECT device, IN PIRP Irp)
{
	// Get our current stack location
	PIO_STACK_LOCATION pLoc = IoGetCurrentIrpStackLocation(Irp);

	// Check that our control code is correct
	//	This *should* change from time to time to
	//	increase security
	if(pLoc->Parameters.DeviceIoControl.IoControlCode == 1000)
	{		
		// Get the contents
		unsigned char* buff = (UCHAR*)Irp->AssociatedIrp.SystemBuffer;
		
		// Create a pointer to connect information
		struct NC_CONNECT_INFO* ncCInfo = (struct NC_CONNECT_INFO*)buff;

		// Check our security
		if(ncCInfo->secCode == EAT_STOOL)
		{
			// Security code passed! - Debug
			NCLOG("Passed security check");

			// Set up image-loading events
			ncImageLoadEventSettings.buff = (char*)MmMapIoSpace(MmGetPhysicalAddress((void*)ncCInfo->ILBuffAddr), (SIZE_T)ncCInfo->ILBuffLen, 0);
			memset(ncImageLoadEventSettings.buff, 0, ncCInfo->ILBuffLen);
			ncImageLoadEventSettings.buffSize = ncCInfo->ILBuffLen;
			ncImageLoadEventSettings.maxEvents = (ncCInfo->ILBuffLen - 1) / sizeof(struct NC_IL_INFO);

#ifdef DEBUG
			// Report sizes
			DbgPrint("[nC]IL Event Length %d | Total Buffered Events %d", sizeof(struct NC_IL_INFO), ncImageLoadEventSettings.maxEvents);

			// Check if there is a fall-off and report it
			if(((ncCInfo->ILBuffLen - 1) % sizeof(struct NC_IL_INFO)) > 0)
				DbgPrint("[nC]ILBuff Falloff! %d bytes can be trimmed!", ((ncCInfo->ILBuffLen - 1) % sizeof(struct NC_IL_INFO)));
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
	// Null out values (reset)
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
		PsRemoveLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE*)&ImageLoadCallback);

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
	cbRegistered = PsSetLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE*)&ImageLoadCallback);

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