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

// Setup buffer pointers
struct NC_IMAGE_CONTAINER* pImageEvents;

char VerifyLink(struct NC_CONNECT_INFO_R* ncRInf)
{
	// Check security
	if(ncRInf->iSecurityCode != NC_LINK_SEC_CODE)
	{
		// Log and break
		LOG2("Link security code failed! (%X)", ncRInf->iSecurityCode);
		return 0;
	}else
		LOG2("Link passed security check");

	// Verify sizes and log
	assert (ncRInf->iNCConnectInfoRSize == sizeof(struct NC_CONNECT_INFO_R));
	assert (ncRInf->iNCImageContainerSize == sizeof(struct NC_IMAGE_CONTAINER));
	assert (ncRInf->iNCImageEventSize == sizeof(struct NC_IMAGE_EVENT));
	LOG3("Struct size assertions passed!");

	// Verify version
	if(ncRInf->iDSLinkVersion > NC_DS_LINK_VERSION)
	{
		// Log and break
		LOG2("Service of link is newer than driver! (%d > %d)", ncRInf->iDSLinkVersion, NC_DS_LINK_VERSION);
		return 0;
	}else if(ncRInf->iDSLinkVersion < NC_DS_LINK_VERSION) {
		// Log and break
		LOG2("Driver of link is newer than service of link! (%d < %d)", ncRInf->iDSLinkVersion, NC_DS_LINK_VERSION);
		return 0;
	}

	// Log
	LOG2("Verified link version");

	// Return true
	return 1;
}

/*
 * Called to initiate a link between the driver and a service
 */
NTSTATUS DrvDevLink(IN PDEVICE_OBJECT device, IN PIRP Irp)
{
	// Setup vars
	PIO_STACK_LOCATION pLoc;
	unsigned char* conBuff;
	struct NC_CONNECT_INFO_R* ncRInf;

	// Log
	LOG("Initiating link");

	// Get current stack location
	pLoc = IoGetCurrentIrpStackLocation(Irp);

	// Switch code (intent)
	switch(pLoc->Parameters.DeviceIoControl.IoControlCode)
	{
	case NC_CONNECTION_CODE_IMAGES:
		// Log
		LOG2("Client attempting to query images.");

		// Register control (init) buffer and feux-point to recv info
		conBuff = (UCHAR*)Irp->AssociatedIrp.SystemBuffer;
		ncRInf = (struct NC_CONNECT_INFO_R*)conBuff;

		// Verify link
		if(VerifyLink(ncRInf) == 0)
			break;
		
		// Log
		LOG3("Mapping buffer space (image)");
		
		// Map
		pImageEvents = (struct NC_IMAGE_CONTAINER*)MmMapIoSpace(MmGetPhysicalAddress((void*)ncRInf->pBuff), (SIZE_T)sizeof(struct NC_IMAGE_CONTAINER), 0);
		
		// Check for null
		if(pImageEvents == NULL)
		{
			// Log and break
			LOG2("Could not map space for the image container. Perhaps the backlog is too big?");
			break;
		}

		// Nullify!
		memset(pImageEvents, 0, sizeof(struct NC_IMAGE_CONTAINER));

		// Log
		LOG("Successfully validated image receiver!");

		// DEBUG: Set our count to 1 to signal we're all good
		pImageEvents->iCount = 1;

		break;
	default:
		// Log
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
NTSTATUS DrvClose(IN PDEVICE_OBJECT obj, IN PIRP Irp)
{
	// Setup vars
	PIO_STACK_LOCATION pLoc;

	// Log
	LOG3("Link is closing");

	// Get current stack location
	pLoc = IoGetCurrentIrpStackLocation(Irp);

	// Switch control code and unmap if need-be
	switch(pLoc->Parameters.DeviceIoControl.IoControlCode)
	{
	case NC_CONNECTION_CODE_IMAGES:
		if(pImageEvents != NULL)
		{
			// Log and unmap
			LOG3("Unmapping images buffer");
			MmUnmapIoSpace(pImageEvents, (SIZE_T)sizeof(struct NC_IMAGE_CONTAINER));
		}
		break;
	default:
		// Log and break
		LOG3("Closing dead (un-authed) link (%d)", pLoc->Parameters.DeviceIoControl.IoControlCode);
		break;
	}

	// Complete request
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	// Return
	return STATUS_SUCCESS;
}

/*
 * Called when a file link is created to
 *	create a pipe connection (CreateFile)
 *
 *	Fixes bug #9
 */
NTSTATUS DrvCreate(IN PDEVICE_OBJECT obj, IN PIRP Irp)
{
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

	// Unmap memory if need be
	if(pImageEvents != NULL)
		MmUnmapIoSpace(pImageEvents, (SIZE_T)sizeof(struct NC_IMAGE_CONTAINER));

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
	driver->MajorFunction[IRP_MJ_CLOSE] = DrvClose;
	driver->MajorFunction[IRP_MJ_CREATE] = DrvCreate;
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