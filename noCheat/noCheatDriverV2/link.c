/************************************************************************/
/* Link Functions                                                       */
/*   Handle link initiation and verification                            */
/************************************************************************/

// Include DDK
#include <ntddk.h>

// Include driver headers
#include "defines.h"
#include "ncDriverDefines.h"
#include "structures.h"
#include "globals.h"

/*
 * Verifies a link when the service connects.
 *	Various security, version, and sizing/alignment
 *	checks are performed here.
 */
char VerifyLink(struct NC_CONNECT_INFO_R* ncRInf)
{
	// Check security
	if(ncRInf->iSecurityCode != NC_LINK_SEC_CODE)
	{
		// Log and break
		LOG2("Link security code failed! (%X)", ncRInf->iSecurityCode);
		return 0;
	}else
		LOG2("Link passed security check");

	// Verify sizes
	NASSERT (ncRInf->iNCConnectInfoRSize == sizeof(struct NC_CONNECT_INFO_R), return 0);
	NASSERT (ncRInf->iNCImageContainerSize == sizeof(struct NC_IMAGE_CONTAINER), return 0);
	NASSERT (ncRInf->iNCImageEventSize == sizeof(struct NC_IMAGE_EVENT), return 0);
	NASSERT (ncRInf->iNCProcessContainerSize == sizeof(struct NC_PROCESS_CONTAINER), return 0);
	NASSERT (ncRInf->iNCProcessEventSize == sizeof(struct NC_PROCESS_EVENT), return 0);

	// Log
	LOG3("Struct size assertions passed!");

	// Verify version
	if(ncRInf->iDSLinkVersion > NC_DS_LINK_VERSION)
	{
		// Log and break
		LOG2("Service of link is newer than driver! (%d > %d)", ncRInf->iDSLinkVersion, NC_DS_LINK_VERSION);
		return 0;
	}else if(ncRInf->iDSLinkVersion < NC_DS_LINK_VERSION) {
		// Log and break
		LOG2("Driver of link is newer than service of link! (%d < %d)", ncRInf->iDSLinkVersion, NC_DS_LINK_VERSION);
		return 0;
	}

	// Log
	LOG2("Verified link version");

	// Return true
	return 1;
}

/*
 * Sets up a link
 */
char SetupLink(PIRP Irp, SIZE_T ContainerSize, struct NC_MAPPED_SPACE* sp)
{
	// Setup vars
	struct NC_CONNECT_INFO_R* ncRInf;

	// Register control (init) buffer and feux-pointer to recv info
	//conBuff = (UCHAR*)Irp->AssociatedIrp.SystemBuffer;
	ncRInf = (struct NC_CONNECT_INFO_R*)Irp->AssociatedIrp.SystemBuffer;

	// Verify link
	if(VerifyLink(ncRInf) == 0)
		return 0;

	// Unmap if already mapped
	if(sp->bMapped == 1)
	{
		LOG2("Unmapping image-load buffer");
		MmUnmapIoSpace(sp->pAddr, sp->iSize);
		sp->bMapped = 0;
		sp->iSize = 0;
	}

	// Log
	LOG3("Mapping buffer space");

	// Map
	sp->pAddr = (void*)MmMapIoSpace(MmGetPhysicalAddress((void*)ncRInf->pBuff), ContainerSize, 0);

	// Check for null
	if(sp->pAddr == NULL)
	{
		// Log and break
		LOG2("Could not map space for the container. Perhaps the backlog is too big?");
		return 0;
	}

	// Signal we've mapped the space
	sp->bMapped = 1;

	// Nullify!
	memset(sp->pAddr, 0, ContainerSize);

	// Set size
	sp->iSize = ContainerSize;

	// Return success
	return 1;
}