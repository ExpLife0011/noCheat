/************************************************************************/
/* Utils header definitions                                             */
/************************************************************************/
#ifndef NCH_UTILS__
#define NCH_UTILS__

// Include DDK
#include <ntddk.h>

/*
 * Converts a unicode string to an ansi string
 *	and moves it into an address
 */
extern "C" VOID UMoveAnsiString(PVOID dest, PUNICODE_STRING str, SIZE_T size);

#endif