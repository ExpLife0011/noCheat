/************************************************************************/
/* IRP_MJ_CLOSE                                                         */
/************************************************************************/

// Include DDK
#include <ntddk.h>

// Include driver headers
#include "defines.h"
#include "globals.h"
#include "link.h"

/*
 * Called when a device disconnects
 *
 *	This does not transfer information.
 */
extern "C" NTSTATUS DrvClose(IN PDEVICE_OBJECT obj, IN PIRP Irp)
{
	// Log
	LOG3("Link is closing.");

	// Unmap memory if need be
	CloseLinks();

	// Falsify link flag
	sSpaces.bLink = 0;

	// Complete request
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	// Return
	return STATUS_SUCCESS;
}
