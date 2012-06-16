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

// Create unmap macro
#define UNMAP(a) Unmap(&sSpaces.##a)

/*
 * Verifies a link when the service connects.
 *	Various security, version, and sizing/alignment
 *	checks are performed here.
 */
extern "C" char VerifyLink(struct NC_CONNECT_INFO_INPUT* ncRInf)
{
	// Check security
	if(ncRInf->iSecurityCode != NC_LINK_SEC_CODE)
	{
		// Log and break
		LOG2("Link security code failed! (%X)", ncRInf->iSecurityCode);
		return 0;
	}else
		LOG2("Link passed security check");

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
 * Tries to map a link
 */
extern "C" VOID TryMapLink(void* src, struct MAP_PARAMS* dest, struct NC_CONNECT_INFO_OUTPUT* returnInf, int aSize)
{
	// Setup variables
	PMDL mdl;

	// Log
	LOG3("Mapping %d bytes", aSize);

	// Get the mdl of the buffer space
	mdl = IoAllocateMdl(src, aSize, 0, 0, NULL);
	
	// Check for null
	if(mdl == NULL)
	{
		LOG3("Could not retrieve MDL list for buffer space!");
		return;
	}

	// Lock and probe pages
	MmProbeAndLockPages(mdl, UserMode, IoModifyAccess);

	// Get system addresses for MDL table and return our mapped buffer
	dest->oContainer = MmGetSystemAddressForMdlSafe(mdl, NormalPagePriority);

	// Check for null
	if(dest->oContainer == NULL)
	{
		LOG3("Error getting physical mappings for virtual buffer!");
		MmUnlockPages(mdl);
		IoFreeMdl(mdl);
		return;         
	}

	// Set parameters
	dest->bMapped = 1;
	dest->iSize = aSize;
	dest->mdl = mdl;
}

/*
 * Unmaps a single map param
 */
VOID Unmap(MAP_PARAMS* params)
{
	// Check if it's mapped
	if(params->bMapped == 0) return;

	// Unlock pages
	MmUnlockPages(params->mdl);

	// Free MDL
	IoFreeMdl(params->mdl);

	// Mark that it's unmapped
	params->bMapped = 0;
}

/*
 * Nullifies all links
 */
extern "C" VOID CloseLinks()
{
	UNMAP(Return);
	UNMAP(Images);
	UNMAP(Processes);
	UNMAP(Threads);
}