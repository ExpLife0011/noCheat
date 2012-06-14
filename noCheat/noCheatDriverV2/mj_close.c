/************************************************************************/
/* IRP_MJ_CLOSE                                                         */
/************************************************************************/

// Include DDK
#include <ntddk.h>

// Include driver headers
#include "defines.h"
#include "globals.h"

/*
 * Called when a device disconnects
 *
 *	This does not transfer information.
 */
NTSTATUS DrvClose(IN PDEVICE_OBJECT obj, IN PIRP Irp)
{
	// Log
	LOG3("Link is closing.");

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

	// Complete request
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	// Return
	return STATUS_SUCCESS;
}
