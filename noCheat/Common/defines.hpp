#ifndef H_DEFINES__
#define H_DEFINES__

/*
 * NC_DLL
 *	Define this if you're building the NC lib
 *	for a DLL instead of an application.
 */
//#define NC_DLL

/*
 * NC_OPTIMIZE_ENCRYPTION
 *	Define this if encryption needs some optimizations
 *	This disables overhead with calling procedures, and 
 *	will mark encryption functions as inline. Undef to
 *	force these functions to be contained in their own
 *	procedures (slower, but may be harder to reconstruct
 *	if someone is trying to recreate the encryption
 *	functionality).
 */
//#define NC_OPTIMIZE_ENCRYPTION

/*
 * NC_END_PASSES
 *	The number of encryption passes
 *	to make
 */
#define NC_ENC_PASSES 3

/*
 * _ncEndPrivkey[16]
 *	The private key used by the encryption functions
 */
static const unsigned char _ncEncPrivkey[] = {0x6e, 0x4A, 0x75, 0x96, 0x67, 0x51, 0x67, 0x18, 0x65, 0x17, 0x74, 0xF3, 0x3a, 0x02, 0x44, 0x99};

/*
 * NC_PRIV_KEY_LENGTH
 *	The length of the (above) priv key
 */
#define NC_PRIV_KEY_LENGTH 16

/*
 * NC_KEY_MAX_LENGTH
 *	The maximum length any pub key will be
 */
#define NC_KEY_MAX_LENGTH 32

  //****************************\\ 
#ifdef NC_DLL
#define NC_LIBEXPORT(a, ...) extern "C" __declspec(dllexport) __VA_ARGS__ a __cdecl
#else
#define NC_LIBEXPORT(a, ...) extern "C" __VA_ARGS__ a __cdecl
#endif
#ifdef NC_OPTIMIZE_ENCRYPTION
#define _NC_OPTENC inline
#else
#define _NC_OPTENC __declspec(noinline)
#endif
#if NC_KEY_MAX_LENGTH < NC_PRIV_KEY_LENGTH
#error (noCheat) Private key length cannot exceed max key length
#endif
#if _WIN32 || _WIN64 // Win32
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif
#if __GNUC__ // GCC
#if __x86_64__ || __ppc64__
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif
//\\****************************//\\

#endif