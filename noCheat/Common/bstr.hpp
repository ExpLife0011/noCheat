#ifndef H_UTIL_BSTR__
#define H_UTIL_BSTR__

#include <windows.h>
#include <WTypes.h>
#include <comdef.h>

#include "defines.hpp"

NC_LIBEXPORT(VOID) ncBStrToCharP(char* outc, BSTR b);

#endif
