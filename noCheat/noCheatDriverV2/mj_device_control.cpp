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

// Define our procedure for mapping
#define MAP_LINK(name, src, dest, size) if(src > 0){ LOG2("Mapping space for: " name); TryMapLink((void*)src, (struct TEMP_MAP_PARAMS*)&dest, &returnInf, size, sizeof(*dest.oContainer)); }

// Define a macro for size assertions
#define CHECK_EVENT_SIZE(a, b) NASSERT((a == sizeof(struct b)), {returnInf.bSizeMismatch = 1; goto WriteReturn;})

/*
 * Called to initiate a link between the driver and a service
 */
extern "C" NTSTATUS DrvDevLink(IN PDEVICE_OBJECT device, IN PIRP Irp)
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

	// Switch code (intent)
	switch(pLoc->Parameters.DeviceIoControl.IoControlCode)
	{
	case NC_CONNECTION_CODE:
		// Log
		LOG2("Connection wants to map containers");

		// Check if we have a connection already


		// Assert size
		NASSERT (pLoc->Parameters.DeviceIoControl.InputBufferLength == sizeof(struct NC_CONNECT_INFO_INPUT), goto WriteReturn);

		// Setup input info
		inputInf = (struct NC_CONNECT_INFO_INPUT*)Irp->AssociatedIrp.SystemBuffer;

		// Memset returnInf
		memset(&returnInf, 0, sizeof(struct NC_CONNECT_INFO_OUTPUT));

		// Attempt to map return value
		//MAP_LINK("Return", inputInf->pReturnInfo, sSpaces.Return, inputInf->iReturnSize);

		// Check to see if there is already another connection
		if(sSpaces.bLink == 1)
		{
			// Set blocked and non-success
			returnInf.bBlocked = 1;
			returnInf.bSuccess = 0;

			// Write and quit
			goto WriteReturn;
		}

		// Verify link
		ret = VerifyLink(inputInf);

		// Test Verification
		if(ret == 1)
			LOG("Successfully validated image receiver!");
		else
		{
			LOG("Could not validate image receiver!");
			returnInf.bAccessDenied = 1;
			returnInf.bSuccess = 0;
			goto WriteReturn;
		}

		// Try to map links
		MAP_LINK("Images", inputInf->pImageLoadContainer, sSpaces.Images, inputInf->iImageContainerSize);
		//MAP_LINK("Processes", inputInf->pProcessCreateContainer, sSpaces.Processes, inputInf->iProcessContainerSize);

		// Check sizes flag
		if(returnInf.bSizeMismatch == 1)
		{
			// Log
			LOG("Size mismatch! Returning failure!");

			// Close all links
			CloseLinks();

			// End
			break;
		}

		// Check event sizes
		CHECK_EVENT_SIZE(inputInf->iImageEventSize, NC_IMAGE_EVENT);
		CHECK_EVENT_SIZE(inputInf->iProcessEventSize, NC_PROCESS_EVENT);

		// Signal a success
		returnInf.bSuccess = 1;

		// Signal that we have a link
		sSpaces.bLink = 1;

		// Write test
		//memset(sSpaces.Images.oContainer, 1, sSpaces.Images.iSize);

WriteReturn:
		// Check/write return information
		if(sSpaces.Return.bMapped == 1)
			memcpy(sSpaces.Return.oContainer, &returnInf, sizeof(struct NC_CONNECT_INFO_OUTPUT));

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