#include <windows.h>
#include <WTypes.h>
#include <comdef.h>

#include "defines.hpp"

NC_LIBEXPORT(VOID) ncBStrToCharP(char* outc, BSTR b)
{
	_bstr_t tmp(b, FALSE);
	const char* t = static_cast<const char *>(tmp);
	memcpy(outc, t, strlen(t));
	SysFreeString(b);
}