/************************************************************************/
/* Driver unload function                                               */
/************************************************************************/

// Include DDK
#include <ntddk.h>

// Include driver headers
#include "defines.h"
#include "globals.h"
#include "callbacks.h"
#include "link.h"


/*
 * DrvUnload
 *	Driver unload point
 */
extern "C" void DrvUnload(IN PDRIVER_OBJECT driver)
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

	// Destroy thread-creation callback
	LOG2("Unregistering thread-creation callback");
	PsRemoveCreateThreadNotifyRoutine((PCREATE_THREAD_NOTIFY_ROUTINE)&ThreadCreateCallback);

	// Unmap memory if need be
	CloseLinks();

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