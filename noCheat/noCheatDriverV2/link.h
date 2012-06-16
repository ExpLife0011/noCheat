/************************************************************************/
/* Link functions declarations                                          */
/************************************************************************/
#ifndef NCH_LINK__
#define NCH_LINK__

// Include DDK
#include <ntddk.h>

// Include structures
#include "structures.h"

/*
 * Verifies a link when the service connects.
 *	Various security, version, and sizing/alignment
 *	checks are performed here.
 */
extern "C" char VerifyLink(struct NC_CONNECT_INFO_INPUT* ncRInf);

/*
 * Tries to map a link
 */
extern "C" VOID TryMapLink(void* src, struct TEMP_MAP_PARAMS* dest, struct NC_CONNECT_INFO_OUTPUT* returnInf, int aSize, SIZE_T size);

/*
 * Nullifies all links
 */
extern "C" VOID CloseLinks();

#endif