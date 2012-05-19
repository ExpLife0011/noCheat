/*
 * Rolling Bitshifters
 */
#ifndef H_ENC_ROLL__
#define H_ENC_ROLL__

#include <windows.h>

#include "defines.hpp"

NC_LIBEXPORT(VOID, _NC_OPTENC) rol8(unsigned char* a, unsigned char b);
NC_LIBEXPORT(VOID, _NC_OPTENC) ror8(unsigned char* a, unsigned char b);
NC_LIBEXPORT(VOID, _NC_OPTENC) rol16(unsigned short* a, unsigned char b);
NC_LIBEXPORT(VOID, _NC_OPTENC) ror16(unsigned short* a, unsigned char b);
NC_LIBEXPORT(VOID, _NC_OPTENC) rol32(unsigned int* a, unsigned char b);
NC_LIBEXPORT(VOID, _NC_OPTENC) ror32(unsigned int* a, unsigned char b);


#endif