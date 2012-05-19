/*
 * noCheat Common Utilities
 */

#ifndef H_NC_COMMON_UTIL__
#define H_NC_COMMON_UTIL__

#include <windows.h>

extern "C"
{
	/*
	 * ncEnableConsole()
	 *	Allocates a console and maps STDOUT/ERR
	 *	NOTE: Does not map input, and only one console can be alloc'd
	 *
	 *	Returns: TRUE on success and FALSE on failure
	 */
	BOOL ncEnableConsole();
}

#endif