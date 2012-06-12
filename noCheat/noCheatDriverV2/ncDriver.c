/************************************************************************/
/*   noCheat Kernel-Mode Driver V2                                      */
/*      This driver is used by the noCheat system to monitor            */
/*      processes being created and to monitor which DLLs or            */
/*      modules they load.                                              */
/*                                                                      */
/*                                                                      */
/*      This kernel-mode driver should not be altered passed            */
/*      its intended purpose, and is to be tested thoroughly            */
/*      before being released to the public.                            */
/*                                                                      */
/*                                                                      */
/*      Due to some key design flaws in the original driver,            */
/*      this 'version 2' has been created to standardize the            */
/*      communication between noCheatD and noCheatS.                    */
/*                                                                      */
/************************************************************************/

// Include the NTDDK header file
#include <ntddk.h>

// Include assertions (insanity checks)
#include <assert.h>

// Include the header defines (used in the service
//	project)
#include "ncDriverDefines.h"

// Debug?
//	Comment out to set to release mode
//	(**THIS MUST BE DONE MANUALLY SINCE VS
//	DOES NOT SIGNAL TO THE BUILD ENVIRONMENTS!**)
#define DEBUG

// Setup the log function
#ifdef DEBUG
#define LOG(a, ...) DbgPrint("[nC] " a "\n", __VA_ARGS__)
#define LOG2(a, ...) LOG("\t- " a, __VA_ARGS__)
#define LOG3(a, ...) LOG("\t\t- " a, __VA_ARGS__)
#else
#define LOG(...)
#define LOG2(...)
#define LOG3(...)
#endif

// Declare our link constants
const WCHAR devicename[] = L"\\Device\\noCheat";
const WCHAR devicelink[] = L"\\DosDevices\\NOCHEAT";

/*
 * Called to initiate a link between the driver and a service
 */
NTSTATUS DrvDevLink(IN PDEVICE_OBJECT device, IN PIRP Irp)
{
	// Setup vars
	PIO_STACK_LOCATION pLoc;

	// Log
	LOG("Initiating link");

	// Get current stack location
	pLoc = IoGetCurrentIrpStackLocation(Irp);

	// Switch code (intent)
	switch(pLoc->Parameters.DeviceIoControl.IoControlCode)
	{
	default:
		LOG("Control connection type not implemented/supported, or an old code has been used (%d)", pLoc->Parameters.DeviceIoControl.IoControlCode);
		break;
	}

	// Log
	LOG2("Completing link initiation");

	// Complete request
	Irp->IoStatus.Status = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	// Return success
	return STATUS_SUCCESS;
}

/*
 * Called when a device connects (sets up buffers
 *	and whatnot) or disconnects
 *
 *	This does not transfer information.
 */
NTSTATUS DrvCreateClose(IN PDEVICE_OBJECT obj, IN PIRP Irp)
{
	// Log
	LOG3("Checking buffers");

	// Complete request
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	// Return
	return STATUS_SUCCESS;
}

/*
 * DrvUnload
 *	Driver unload point
 */
void DrvUnload(IN PDRIVER_OBJECT driver)
{
	// Setup vars
	UNICODE_STRING devLink;

	// Log entry
	LOG("Unloading driver");

	// Convert devlink string
	RtlInitUnicodeString(&devLink, devicelink);

	// Delete symlink
	IoDeleteSymbolicLink(&devLink);

	// Delete device
	IoDeleteDevice(driver->DeviceObject);

	// Log exit
	LOG("Driver unloaded");
}

/*
 * DriverEntry
 *	Driver entry point
 */
NTSTATUS DriverEntry(IN PDRIVER_OBJECT driver, IN PUNICODE_STRING path)
{
	// Setup vars
	UNICODE_STRING devLink, devName;
	PDEVICE_OBJECT devObj = NULL;
	NTSTATUS ntsReturn;

	// Log Entry
	LOG("Driver Entry");

	// Setup driver unload function
	driver->DriverUnload = DrvUnload;

	// Convert our strings to Unicode
	LOG3("Converting device/link strings");
	RtlInitUnicodeString(&devLink, devicelink);
	RtlInitUnicodeString(&devName, devicename);

	// Create device
	LOG2("Creating Device");
	assert ((256 >= sizeof(struct NC_CONNECT_INFO_R)) && (256 >= sizeof(struct NC_CONNECT_INFO_S)));
	ntsReturn = IoCreateDevice(driver, 256, &devName, FILE_DEVICE_UNKNOWN, 0, TRUE, &devObj);

	// Check the status of the device creation
	if(ntsReturn == STATUS_SUCCESS)
	{
		// Log
		LOG2("Successfully created IO Device");

	}else {
		// Switch the error message
		switch(ntsReturn)
		{
		case STATUS_INSUFFICIENT_RESOURCES:
			LOG2("Could not create device: Insufficient Resources");
			break;
		case STATUS_OBJECT_NAME_EXISTS:
			LOG2("Could not create device: Object name already exists");
			break;
		case STATUS_OBJECT_NAME_COLLISION:
			LOG2("Could not create device: Object name collision");
			break;
		default:
			LOG2("Could not create device: Unknown error");
			break;
		}

		// Log and terminate
		LOG("Terminating!");
		goto ErrFreeUni;
	}

	// Create symlink
	ntsReturn = IoCreateSymbolicLink(&devLink, &devName);
	
	// Test return
	if(ntsReturn == STATUS_SUCCESS)
	{
		LOG2("Successfully created symlink to device");
	}else{
		LOG2("Could not create symlink to device!");
		LOG("Terminating!");
		goto ErrFreeDev;
	}

	// Setup major functions
	driver->MajorFunction[IRP_MJ_CREATE] = DrvCreateClose;
	driver->MajorFunction[IRP_MJ_CLOSE] = DrvCreateClose;
	driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DrvDevLink;


// Jump to success
goto Success;

/*---
 * START ERROR BLOCK *
				  ---*/
ErrFreeDev: // Occurs when the symlink creation fails
IoDeleteDevice(devObj);

ErrFreeUni: // Occurs when either the symlink or device creation fails
return STATUS_SEVERITY_ERROR;

Success:
	// Log Exit
	LOG("Driver now online (returning success)");

	// Return success
	return STATUS_SUCCESS;
}