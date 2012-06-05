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

	/*
	 * ncKeyShift()
	 *	Takes a byte sequence and forms it into a key
	 */
	VOID ncKeyShift(char* keybuf, int len);

	/*
	 * ncKeyExpand()
	 *	Expands a key out to a full length
	 */
	VOID ncKeyExpand(char* key, int maxlen, int initlen);

	/*
	 * ncCreateUsageKey()
	 *	Creates a usage key based off of the private/public keys
	 *	Ror rolls the bits to the right
	 */
	VOID ncCreateUsageKey(unsigned char* output, unsigned char* key, int len, int rora);

	/*
	 * ncEncrypt_1() / ncDecrypt_1()
	 *	Type-1 encryption methods
	 */
	VOID ncEncrypt_1(unsigned char* outbuf, int len, unsigned char* key, int klen);
	VOID ncDecrypt_1(unsigned char* outbuf, int len, unsigned char* key, int klen);
}

#endif