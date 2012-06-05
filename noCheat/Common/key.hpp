/*
 * Key Tools
 */
#ifndef H_ENC_KEY__
#define H_ENC_KEY__

#include <windows.h>

#include "defines.hpp"

NC_LIBEXPORT(VOID) ncKeyShift(unsigned char* keybuf, int len);
NC_LIBEXPORT(VOID) ncKeyExpand(unsigned char* key, int initlen);
NC_LIBEXPORT(VOID) ncCreateUsageKey(unsigned char* output, unsigned char* key, int len, int rora);

#endif