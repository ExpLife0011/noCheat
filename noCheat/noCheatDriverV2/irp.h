/************************************************************************/
/* IRP function declarations                                            */
/************************************************************************/
#ifndef NCH_IRP__
#define NCH_IRP__

// Include DDK
#include <ntddk.h>

/*
 * Called when a file link is created to
 *	create a pipe connection (CreateFile)
 *
 *	Fixes bug #9
 */
NTSTATUS DrvCreate(IN PDEVICE_OBJECT obj, IN PIRP Irp);

/*
 * Called when a device disconnects
 *
 *	This does not transfer information.
 */
NTSTATUS DrvClose(IN PDEVICE_OBJECT obj, IN PIRP Irp);

/*
 * Called to initiate a link between the driver and a service
 */
NTSTATUS DrvDevLink(IN PDEVICE_OBJECT device, IN PIRP Irp);

/*
 * Called when the system shuts down or
 *	restarts
 */
NTSTATUS SysShutdownRestart(IN PDEVICE_OBJECT device, IN PIRP Irp);

#endif