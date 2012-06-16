/************************************************************************/
/* Utility functions                                                    */
/*   These functions provide useful helper functionality                */
/************************************************************************/

// Include DDK
#include <ntddk.h>

/*
 * Converts a unicode string to an ansi string
 *	and moves it into an address
 */
extern "C" VOID UMoveAnsiString(PVOID dest, PUNICODE_STRING str, SIZE_T size)
{
	// Setup var
	ANSI_STRING ansi;

	// Memset buffer
	memset(dest, 0, size);

	// Convert to Ansi String
	RtlUnicodeStringToAnsiString(&ansi, str, 1);

	// Memcpy
	memcpy(dest, ansi.Buffer, ansi.Length);

	// Free
	RtlFreeAnsiString(&ansi);
}