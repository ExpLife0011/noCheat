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
extern "C" NTSTATUS SysShutdownRestart(IN PDEVICE_OBJECT device, IN PIRP Irp)
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