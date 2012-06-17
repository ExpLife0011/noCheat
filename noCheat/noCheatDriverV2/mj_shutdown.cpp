/************************************************************************/
/* IRP_MJ_SHUTDOWN                                                      */
/************************************************************************/

// Include DDK
#include <ntddk.h>

// Include driver headers
#include "unload.h"
#include "defines.h"

/*
 * Called when the system shuts down or
 *	restarts
 */
extern "C" NTSTATUS SysShutdownRestart(PDEVICE_OBJECT device, PIRP Irp)
{
	// Log
	LOG("System is shutting down/restarting. Killing driver...");

	KeBugCheck(0xAC1D444);

	// Call DrvUnload
	DrvUnload(device->DriverObject);

	// Complete request
	Irp->IoStatus.Status = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	// Return success
	return STATUS_SUCCESS;
}