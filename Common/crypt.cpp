#include <windows.h>

#include "defines.hpp"
#include "crypt.hpp"

NC_LIBEXPORT(VOID) ncEncrypt1(char* outbuf, int len, char* key, int len)
{
	// do 3 times:
	for(int pass = 0; pass < NC_ENC_PASSES; pass++)
	{
		// Xor
		// Shift left 4
		// xor 
		// shift right
	}
}