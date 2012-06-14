/************************************************************************/
/* Link functions declarations                                          */
/************************************************************************/
#ifndef NCH_LINK__
#define NCH_LINK__

// Include DDK
#include <ntddk.h>

/*
 * Verifies a link when the service connects.
 *	Various security, version, and sizing/alignment
 *	checks are performed here.
 */
char VerifyLink(struct NC_CONNECT_INFO_INPUT* ncRInf);

#endif