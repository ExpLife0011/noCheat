/*
 * noCheat Encryption Functionality
 */
#ifndef H_NC_COMMON_ENC__
#define H_NC_COMMON_ENC__

#include <windows.h>

extern "C"
{
	/*
	 * rolx() / rorx()
	 *	Rolls (wrap-bitshifts) bits left/right
	 */
	VOID rol8(unsigned char* a, unsigned char b);
	VOID ror8(unsigned char* a, unsigned char b);
	VOID rol16(unsigned short* a, unsigned char b);
	VOID ror16(unsigned short* a, unsigned char b);
	VOID rol32(unsigned int* a, unsigned char b);
	VOID ror32(unsigned int* a, unsigned char b);
}

#endif