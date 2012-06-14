/************************************************************************/
/* Driver unload function                                               */
/************************************************************************/

// Include DDK
#include <ntddk.h>

// Include driver headers
#include "defines.h"
#include "globals.h"
#include "callbacks.h"


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
	if(sSpaces.Image.bMapped == 1)
	{
		LOG2("Unmapping image-load buffer");
		MmUnmapIoSpace(sSpaces.Image.pAddr, (SIZE_T)sSpaces.Image.iSize);
		sSpaces.Image.bMapped = 0;
	}
	if(sSpaces.Process.bMapped == 1)
	{
		LOG2("Unmapping process buffer");
		MmUnmapIoSpace(sSpaces.Process.pAddr, (SIZE_T)sSpaces.Process.iSize);
		sSpaces.Process.bMapped = 0;
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