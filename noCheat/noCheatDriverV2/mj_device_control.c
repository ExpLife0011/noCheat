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

	// Log
	LOG("Initiating link");
	LOG("D: %d", &sSpaces);

	// Get current stack location
	pLoc = IoGetCurrentIrpStackLocation(Irp);

	// Assert size
	NASSERT (pLoc->Parameters.DeviceIoControl.InputBufferLength == sizeof(struct NC_CONNECT_INFO_R), goto CompleteDIORequest);

	// Switch code (intent)
	switch(pLoc->Parameters.DeviceIoControl.IoControlCode)
	{
	case NC_CONNECTION_CODE_PROCESSES:
		// Log
		LOG2("Client attempting to query processes");

		// Try to setup link
		ret = SetupLink(Irp, sizeof(struct NC_PROCESS_CONTAINER), &(sSpaces.Process));

		// Log
		if(ret == 1)
			LOG("Successfully validated process creation receiver!");
		else
			LOG("Could not validate process creation receiver!");

		break;
	case NC_CONNECTION_CODE_IMAGES:
		// Log
		LOG2("Client attempting to query images.");

		// Try to setup link
		ret = SetupLink(Irp, sizeof(struct NC_IMAGE_CONTAINER), &(sSpaces.Image));

		// Log
		if(ret == 1)
			LOG("Successfully validated image receiver!");
		else
			LOG("Could not validate image receiver!");

		break;
	default:
		// Log
		LOG("Control connection type not implemented/supported, or an old code has been used (%d)", pLoc->Parameters.DeviceIoControl.IoControlCode);
		break;
	}

	// Log
	LOG2("Completing link initiation");

CompleteDIORequest:

	// Complete request
	Irp->IoStatus.Status = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	// Return success
	return STATUS_SUCCESS;
}