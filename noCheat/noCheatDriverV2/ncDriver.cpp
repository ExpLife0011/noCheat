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

// Include the header defines (used in the service
//	project)
#include "ncDriverDefines.h"

// Driver specific headers
#include "defines.h"		// Basic defines
#include "callbacks.h"		// Callback functions
#include "globals.h"		// Include globals
#include "irp.h"			// IRP functions
#include "unload.h"			// Unload point

// Define entry as extern "C" for driver
extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT driver, IN PUNICODE_STRING path);

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

	// Setup mapped spaces object
	memset(&sSpaces, 0, sizeof(sSpaces));

	// Convert our strings to Unicode
	LOG3("Converting device/link strings");
	RtlInitUnicodeString(&devLink, devicelink);
	RtlInitUnicodeString(&devName, devicename);

#pragma region Create Device	
	// Check sizes
	NASSERT ((256 >= sizeof(struct NC_CONNECT_INFO_INPUT)) && (256 >= sizeof(struct NC_CONNECT_INFO_OUTPUT)), goto ErrFreeUni);
	
	// Log
	LOG2("Creating Device");

	// Create device
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

#pragma region Setup Process Creation Callback
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

#pragma region Setup Thread Creation Callback
	// Setup thread creation callback
	ntsReturn = PsSetCreateThreadNotifyRoutine((PCREATE_THREAD_NOTIFY_ROUTINE)&ThreadCreateCallback);

	// Check return
	switch(ntsReturn)
	{
	case STATUS_SUCCESS:
		LOG2("Registered Thread Creatino Callback Successfully!");
		break;
	default:
		LOG2("Could not register thread creation callback: Unknown error");
		goto ErrUnregProcess;
	}
#pragma endregion

	// Setup major functions
	driver->MajorFunction[IRP_MJ_CLOSE] = DrvClose;
	driver->MajorFunction[IRP_MJ_CREATE] = DrvCreate;
	driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DrvDevLink;
	driver->MajorFunction[IRP_MJ_SHUTDOWN] = SysShutdownRestart;

	
// Jump to success
goto Success;

/*---
 * START ERROR BLOCK *
				  ---*/
ErrUnregProcess: // Occurs when the thread creation callback fails
PsRemoveCreateThreadNotifyRoutine((PCREATE_THREAD_NOTIFY_ROUTINE)&ThreadCreateCallback);

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