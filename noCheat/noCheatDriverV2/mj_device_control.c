/************************************************************************/
/* IRP_MJ_DEVICE_CONTROL                                                */
/************************************************************************/

// Include DDK
#include <ntddk.h>

// Include driver headers
#include "defines.h"
#include "globals.h"
#include "ncDriverDefines.h"
#include "link.h"

/*
 * Called to initiate a link between the driver and a service
 */
NTSTATUS DrvDevLink(IN PDEVICE_OBJECT device, IN PIRP Irp)
{
	// Setup vars
	PIO_STACK_LOCATION pLoc;
	unsigned char* conBuff;
	char ret;
	struct NC_CONNECT_INFO_INPUT* inputInf;
	struct NC_CONNECT_INFO_OUTPUT returnInf;

	// Log
	LOG("Initiating link");

	// Get current stack location
	pLoc = IoGetCurrentIrpStackLocation(Irp);

	// Assert size
	NASSERT (pLoc->Parameters.DeviceIoControl.InputBufferLength == sizeof(struct NC_CONNECT_INFO_INPUT), goto DIORequestFailure);

	// Setup input info
	inputInf = (struct NC_CONNECT_INFO_INPUT*)Irp->AssociatedIrp.SystemBuffer;

	// Memset returnInf
	memset(&returnInf, 0, sizeof(struct NC_CONNECT_INFO_OUTPUT));

	// Switch code (intent)
	switch(pLoc->Parameters.DeviceIoControl.IoControlCode)
	{
	case NC_CONNECTION_CODE:
		// Log
		LOG2("Connection wants to map containers");

		// Verify link
		ret = SetupLink(Irp, sizeof(struct NC_IMAGE_CONTAINER), &(sSpaces.Image));
		

		// Log
		if(ret == 1)
		{
			LOG("Successfully validated image receiver!");
			inputInf->sReturn.cSuccess = 1;
		}
		else
			LOG("Could not validate image receiver!");

		break;
	default:
		// Log
		LOG("Control connection type not implemented/supported, or an old code has been used (%d)", pLoc->Parameters.DeviceIoControl.IoControlCode);
	}

	// Log
	LOG2("Completing link initiation");

	// Complete request
	Irp->IoStatus.Status = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	// Return success
	return STATUS_SUCCESS;
}