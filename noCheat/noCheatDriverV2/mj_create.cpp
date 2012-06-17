/************************************************************************/
/* IRP_MJ_CREATE                                                        */
/************************************************************************/

// Include DDK
#include <ntddk.h>

/*
 * Called when a file link is created to
 *	create a pipe connection (CreateFile)
 *
 *	Fixes bug #9
 */
extern "C" NTSTATUS DrvCreate(PDEVICE_OBJECT obj, PIRP Irp)
{
	// Unreferenced Params
	UNREFERENCED_PARAMETER(obj);

	// Complete request
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	// Return
	return STATUS_SUCCESS;
}
