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
extern "C" NTSTATUS DrvCreate(PDEVICE_OBJECT obj, PIRP Irp);

/*
 * Called when a device disconnects
 *
 *	This does not transfer information.
 */
extern "C" NTSTATUS DrvClose(PDEVICE_OBJECT obj, PIRP Irp);

/*
 * Called to initiate a link between the driver and a service
 */
extern "C" NTSTATUS DrvDevLink(PDEVICE_OBJECT device, PIRP Irp);

/*
 * Called when the system shuts down or
 *	restarts
 */
extern "C" NTSTATUS SysShutdownRestart(PDEVICE_OBJECT device, PIRP Irp);

#endif