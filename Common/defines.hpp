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
//\\****************************//\\

#endif