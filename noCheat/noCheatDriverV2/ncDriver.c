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

// Include the filesystem functionality
#include <Ntifs.h>

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

// Set up unicode processing method
#define MOVEANSI(vA, vB) UMoveAnsiString(&(vA), vB, sizeof(vA))

// Setup which callback we should use for process creation detection
#if (NTDDI_VERSION >= NTDDI_VISTASP1)
#define NC_PCN_EXTENDED
#define NC_PROCESSCREATE_NOTIFY(vA, vB) PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)&vA, vB);
#else
#define NC_PROCESSCREATE_NOTIFY(vA, vB) PsSetCreateProcessNotifyRoutine((PCREATE_PROCESS_NOTIFY_ROUTINE)&vA, vB);
#endif

// Declare our link constants
const WCHAR devicename[] = L"\\Device\\noCheat";
const WCHAR devicelink[] = L"\\DosDevices\\NOCHEAT";

// Setup buffer pointers
static struct NC_IMAGE_CONTAINER* pImageEvents;
static struct NC_PROCESS_CONTAINER* pProcessEvents;


/*
 * Converts a unicode string to an ansi string
 *	and moves it into an address
 */
VOID UMoveAnsiString(PVOID dest, PUNICODE_STRING str, SIZE_T size)
{
	// Setup var
	ANSI_STRING ansi;

	// Memset buffer
	memset(dest, 0, size);

	// Convert to Ansi String
	RtlUnicodeStringToAnsiString(&ansi, str, 1);

	// Memcpy
	memcpy(dest, ansi.Buffer, ansi.Length);

	// Free
	RtlFreeAnsiString(&ansi);
}

#ifdef NC_PCN_EXTENDED
/*
 * Called when a process is created
 *	(EXTENDED)
 */
VOID ProcessCreateCallback(PEPROCESS Process, HANDLE ProcessId, PPS_CREATE_NOTIFY_INFO CreateInfo)
{
	// Setup vars
	struct NC_PROCESS_EVENT pe;

	// Check to see if there is a link and return if there is not
	if(pProcessEvents == NULL) return;

	// Check for overflow
	if(pProcessEvents->iCount >= NC_EVENT_BACKLOG)
	{
		// Log and return
		LOG2("Reached process creation event backlog limit!");
		return;
	}

	// Set up new process object
	pe.bExtended = 1;
	pe.iPID = (unsigned __int32)ProcessId;
	pe.iCallingThread.iUniqueProcess = (unsigned __int32)CreateInfo->CreatingThreadId.UniqueProcess;
	pe.iCallingThread.iUniqueThread = (unsigned __int32)CreateInfo->CreatingThreadId.UniqueThread;
	pe.iParentPID = (unsigned __int32)CreateInfo->ParentProcessId;
	
	// Get Process File Name	
	MOVEANSI(pe.szProcessFileName, (PUNICODE_STRING)CreateInfo->ImageFileName);

	// Get Command Line
	MOVEANSI(pe.szCommandLine, (PUNICODE_STRING)CreateInfo->CommandLine);

	// Get Image Name
	MOVEANSI(pe.szFileName, &CreateInfo->FileObject->FileName);

	// Assign to memory
	pProcessEvents->oEvents[pProcessEvents->iCount] = pe;

	// Increment count
	pProcessEvents->iCount = pProcessEvents->iCount + 1;
}

#endif
#ifndef NC_PCN_EXTENDED
/*
 * Called when a process is created
 *	(BASIC / PREVISTA)
 */
VOID ProcessCreateCallback(HANDLE ParentId, HANDLE ProcessId, BOOLEAN Create)
{
	// Setup vars
	struct NC_PROCESS_EVENT pe;

	// Check to see if the process is being destroyed and return if so
	if(!Create) return;

	// Check to see if there is a link and return if there is not
	if(pProcessEvents == NULL) return;

	// Check for overflow
	if(pProcessEvents->iCount >= NC_EVENT_BACKLOG)
	{
		// Log and return
		LOG2("Reached process creation event backlog limit!");
		return;
	}

	// Set up new process object
	pe.bExtended = 0;
	pe.iPID = (unsigned __int32)ProcessId;
	pe.iParentPID = (unsigned __int32)ParentId;

	// Assign to memory
	pProcessEvents->oEvents[pProcessEvents->iCount] = pe;

	// Increment count
	pProcessEvents->iCount = pProcessEvents->iCount + 1;
}

#endif
/*
 * Called whenever an image (DLL or EXE) is loaded
 */
VOID ImageLoadCallback(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo)
{
	// Setup vars
	struct NC_IMAGE_EVENT ie;
	ANSI_STRING ansi;

	// Check to see there is a link and return if there is not
	if(pImageEvents == NULL) return;

	// Check for overflow
	if(pImageEvents->iCount >= NC_EVENT_BACKLOG)
	{
		// Log and return
		LOG2("Reached image event backlog limit!");
		return;
	}

	// Set up a new image object
	ie.iPID = (unsigned __int32)ProcessId;
	ie.bKernelLand = (unsigned char)ImageInfo->SystemModeImage;
	ie.iImageBase = (unsigned __int64)ImageInfo->ImageBase;
	ie.iSize = (unsigned __int32)ImageInfo->ImageSize;

	// Get Image name
	MOVEANSI(ie.szImageName, FullImageName);

	// Assign to memory
	pImageEvents->oEvents[pImageEvents->iCount] = ie;

	// Increment count
	pImageEvents->iCount = pImageEvents->iCount + 1;

	// Log
	LOG3("Image (%d): %wZ", ProcessId, FullImageName);
}

/*
 * Verifies a link when the service connects.
 *	Various security, version, and sizing/alignment
 *	checks are performed here.
 */
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
	case NC_CONNECTION_CODE_PROCESSES:
		// Log
		LOG2("Client attempting to query processes");

		// Register control (init) buffer and feux-pointer to recv info
		conBuff = (UCHAR*)Irp->AssociatedIrp.SystemBuffer;
		ncRInf = (struct NC_CONNECT_INFO_R*)conBuff;

		// Verify Link
		if(VerifyLink(ncRInf) == 0)
			break;

		// Log
		LOG3("Mapping buffer space (process)");

		// Map
		pProcessEvents = (struct NC_PROCESS_CONTAINER*)MmMapIoSpace(MmGetPhysicalAddress((void*)ncRInf->pBuff), (SIZE_T)sizeof(struct NC_PROCESS_CONTAINER), 0);

		// Check for null
		if(pProcessEvents == NULL)
		{
			// Log and break
			LOG2("Could not map space for the process container. Perhaps the backlog is too big?");
			break;
		}

		// Nullify!
		memset(pProcessEvents, 0, sizeof(struct NC_PROCESS_CONTAINER));

		// Log
		LOG("Successfully validated process receiver!");

		break;
	case NC_CONNECTION_CODE_IMAGES:
		// Log
		LOG2("Client attempting to query images.");

		// Register control (init) buffer and feux-pointer to recv info
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
 * Called when a device disconnects
 *
 *	This does not transfer information.
 */
NTSTATUS DrvClose(IN PDEVICE_OBJECT obj, IN PIRP Irp)
{
	// Log
	LOG3("Link is closing.");

	// Unmap links if needbe
	if(pImageEvents != NULL)
		MmUnmapIoSpace(pImageEvents, sizeof(struct NC_IMAGE_CONTAINER));
	if(pProcessEvents != NULL)
		MmUnmapIoSpace(pProcessEvents, sizeof(struct NC_PROCESS_CONTAINER));

	// Set pointers to null
	pImageEvents = NULL;
	pProcessEvents = NULL;

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

	// Destroy image-load callback
	LOG2("Unregistering image-load callback");
	PsRemoveLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE)&ImageLoadCallback);

	// Destroy process-creation callback
	LOG2("Unregistering process-creation callback");
	NC_PROCESSCREATE_NOTIFY(ProcessCreateCallback, 1);

	// Unmap memory if need be
	if(pImageEvents != NULL)
	{
		LOG2("Unmapping image-load buffer");
		MmUnmapIoSpace(pImageEvents, (SIZE_T)sizeof(struct NC_IMAGE_CONTAINER));
	}
	if(pProcessEvents != NULL)
	{
		LOG2("Unmapping process-load buffer");
		MmUnmapIoSpace(pProcessEvents, (SIZE_T)sizeof(struct NC_PROCESS_CONTAINER));
	}

	// Convert devlink string
	RtlInitUnicodeString(&devLink, devicelink);

	// Delete symlink
	LOG2("Deleting sym-link to device");
	IoDeleteSymbolicLink(&devLink);

	// Delete device
	LOG2("Deleting device");
	IoDeleteDevice(driver->DeviceObject);

	// Log exit
	LOG("Driver unloaded");
}

/*
 * Called when the system shuts down or
 *	restarts
 */
NTSTATUS SysShutdownRestart(IN PDEVICE_OBJECT device, IN PIRP Irp)
{
	// Log
	LOG("System is shutting down/restarting. Killing driver...");

	// Call DrvUnload
	DrvUnload(device->DriverObject);

	// Complete request
	Irp->IoStatus.Status = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	// Return success
	return STATUS_SUCCESS;
}

/*
 * DriverEntry
 *	Driver entry point
 */
NTSTATUS DriverEntry(IN PDRIVER_OBJECT driver, IN PUNICODE_STRING path)
{
#pragma region Setup Vars
	// Setup vars
	UNICODE_STRING devLink, devName;
	PDEVICE_OBJECT devObj = NULL;
	NTSTATUS ntsReturn;
#pragma endregion

	// Log Entry
	LOG("Driver Entry");

	// Setup driver unload function
	driver->DriverUnload = DrvUnload;

	// Convert our strings to Unicode
	LOG3("Converting device/link strings");
	RtlInitUnicodeString(&devLink, devicelink);
	RtlInitUnicodeString(&devName, devicename);

#pragma region Create Device
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
#pragma endregion

#pragma region Create Symlink
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
	driver->MajorFunction[IRP_MJ_SHUTDOWN] = SysShutdownRestart;
#pragma endregion

#pragma region Setup Image Callback
	// Setup image callback
	ntsReturn = PsSetLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE)&ImageLoadCallback);

	// Check success
	switch(ntsReturn)
	{
	case STATUS_SUCCESS:
		LOG2("Registered Image-Load Callback Successfully!");
		break;
	case STATUS_INSUFFICIENT_RESOURCES:
		LOG2("Could not register image-load callback: Insufficient Resources. Terminating!");
		goto ErrFreeSymLink;
	default:
		LOG2("Could not register image-load callback: Unknown Error (%d). Terminating!", ntsReturn);
		goto ErrFreeSymLink;
	}
#pragma endregion

#pragma region Setup ProcessEx Creation Callback
	// Setup process creation callback
	ntsReturn = NC_PROCESSCREATE_NOTIFY(ProcessCreateCallback, 0);

	// Check return
	switch(ntsReturn)
	{
	case STATUS_SUCCESS:
		LOG2("Registered Process Creation Callback Successfully!");
		break;
	case STATUS_INVALID_PARAMETER:
		LOG2("Could not register process creation callback: Invalid Parameter (Already registered? Too many registrations?)");
		goto ErrUnregIL;
#ifdef NC_PCN_EXTENDED
	case STATUS_ACCESS_DENIED:
		LOG2("Could not register process creation callback: Access Denied");
		goto ErrUnregIL;
#endif
	default:
		LOG2("Could not register process creation callback: Unknown error");
		goto ErrUnregIL;
	}
#pragma endregion

// Jump to success
goto Success;

/*---
 * START ERROR BLOCK *
				  ---*/
ErrUnregIL: // Occurs when the process creation callback fails
PsRemoveLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE)&ImageLoadCallback);

ErrFreeSymLink: // Occurs when the image-load callback fails
IoDeleteSymbolicLink(&devLink);

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