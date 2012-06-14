/************************************************************************/
/* Unload point declaration                                             */
/************************************************************************/
#ifndef NCH_UNLOAD__
#define NCH_UNLOAD__

// Include DDK
#include <ntddk.h>

/*
 * DrvUnload
 *	Driver unload point
 */
void DrvUnload(IN PDRIVER_OBJECT driver);


#endif