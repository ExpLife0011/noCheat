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
const DWORD EAT_STOOL = 0xEA757001; // EAT_STOOL - Yum.

struct NC_CONNECT_INFO
{
	DWORD secCode; // The security code - currently EAT_STOOL
	DWORD addr;	// The address to map to
};

/*
 * Our condensed image load struct
 *	that is passed back to the user-land service
 */
struct NC_IL_INFO
{
	UNICODE_STRING puszFullImageName;
	HANDLE hwndProcessId;
	PIMAGE_INFO pinfImageInfo;
};

NTSTATUS cbRegistered;

/*
 * Image Load Callback
 */
VOID ImageLoadCallback(
							  __in_opt PUNICODE_STRING FullImageName,
							  __in HANDLE  ProcessId,
							  __in PIMAGE_INFO  ImageInfo
							  )
{
	// Define/populate our condensed struct to be passed
	//	back to the user-land noCheat service
	struct NC_IL_INFO ncInf;
	ncInf.hwndProcessId = ProcessId;
	ncInf.pinfImageInfo = ImageInfo;
	ncInf.puszFullImageName = *FullImageName;

	// Debug
#ifdef DEBUG
	DbgPrint("[nC]Image(%d): %wZ", ProcessId, &ncInf.puszFullImageName);
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
		// Debug
		NCLOG("Received link");
		
		// Get the contents
		UCHAR* buff = (UCHAR*)Irp->AssociatedIrp.SystemBuffer;
		
		// Create a pointer to connect information
		NC_CONNECT_INFO* ncCInfo = (NC_CONNECT_INFO*)buff;

		// Check our security
		if(ncCInfo->secCode == EAT_STOOL)
		{
			// Security code passed! - Debug
			NCLOG("Passed security check");

			// Map address
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
NTSTATUS DrvCreateClose(IN PDEVICE_OBJECT, IN PIRP Irp)
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

	// Convert our strings
	RtlInitUnicodeString(&devLink, devicelink);
	RtlInitUnicodeString(&devName, devicename);

	// Create device
	IoCreateDevice(driver, 256, &devName, FILE_DEVICE_UNKNOWN, 0, TRUE, &devObj);

	// Create link
	IoCreateSymbolicLink(&devLink, &devName);

	// Set major functions
	driver->MajorFunction[IRP_MJ_CREATE] = DrvCreateClose;
	driver->MajorFunction[IRP_MJ_CLOSE] = DrvCreateClose;


	// Set unload function
	driver->DriverUnload = DrvUnload;

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