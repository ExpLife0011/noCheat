#include <windows.h>
#include <stdlib.h>

#include "defines.hpp"
#include "roll.hpp"


NC_LIBEXPORT(VOID, _NC_OPTENC) rol8(unsigned char* a, unsigned char b)
{
	unsigned int t; 
	t = *a; 
	for(int i=0; i < b; i++) { 
		t = t << 1; 
		if(t & 256) 
			t = t | 1;
	} 
	*a = (unsigned char)t;
}

NC_LIBEXPORT(VOID, _NC_OPTENC) ror8(unsigned char* a, unsigned char b)
{
	rol8(a, 8-b);
}

NC_LIBEXPORT(VOID, _NC_OPTENC) rol16(unsigned short* a, unsigned char b)
{
	*a = _rotl(*a, b);
}

NC_LIBEXPORT(VOID, _NC_OPTENC) ror16(unsigned short* a, unsigned char b)
{
	*a = _rotr(*a, b);
}

NC_LIBEXPORT(VOID, _NC_OPTENC) rol32(unsigned int* a, unsigned char b)
{
	*a = _rotl(*a, b);
}

NC_LIBEXPORT(VOID, _NC_OPTENC) ror32(unsigned int* a, unsigned char b)
{
	*a = _rotr(*a, b);
}