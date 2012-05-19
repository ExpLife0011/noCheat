#include <windows.h>

#include "defines.hpp"
#include "key.hpp"
#include "roll.hpp"

NC_LIBEXPORT(VOID) key_shift(char* keybuf, int len)
{
	for(int i = 0; i < (NC_ENC_PASSES - 1); i++)
	{
		bool f = false;
		for(int l = 0; l < len; l++)
		{
			if(f)
				rol8(&keybuf[l], 2);
			else
				ror8(&keybuf[2], 3);
			f = !f;
		}
	}
}