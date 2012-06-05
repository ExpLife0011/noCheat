#include <windows.h>

#include "defines.hpp"
#include "key.hpp"
#include "roll.hpp"

NC_LIBEXPORT(VOID) ncKeyShift(unsigned char* keybuf, int len)
{
	for(int i = 0; i < (NC_ENC_PASSES - 1); i++)
	{
		bool f = false;
		for(int l = 0; l < len; l++)
		{
			if(f)
				rol8((unsigned char*)&keybuf[l], 2);
			else
				ror8((unsigned char*)&keybuf[2], 3);
			f = !f;
		}
	}
}

NC_LIBEXPORT(VOID) ncKeyExpand(unsigned char* key, int initlen)
{
	int abspos = initlen;
	int curkpos = 0;
	while(abspos < NC_KEY_MAX_LENGTH)
	{
		key[abspos] = key[curkpos];
		++abspos;
		++curkpos;
		if(curkpos >= initlen)
			curkpos = 0;
	}
}

NC_LIBEXPORT(VOID) ncCreateUsageKey(unsigned char* output, unsigned char* key, int len, int rora)
{
	// Setup priv key
	unsigned char apkey[NC_KEY_MAX_LENGTH];
	memcpy(&apkey, &_ncEncPrivkey, NC_PRIV_KEY_LENGTH);
	
	// Setup public key
	unsigned char akey[NC_KEY_MAX_LENGTH];
	memcpy(&akey, key, len);

	// Expand keys
	ncKeyExpand((unsigned char*)&apkey, NC_PRIV_KEY_LENGTH);
	ncKeyExpand((unsigned char*)&akey[0], len);

	// Create usage key
	for(int ai = 0; ai < NC_KEY_MAX_LENGTH; ai++)
	{

		output[ai] = (apkey[ai] ^ akey[ai]) ^ 0xFF;
		ror8(&output[ai], rora);
	}
}