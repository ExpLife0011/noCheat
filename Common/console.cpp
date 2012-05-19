#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>

#include "defines.hpp"
#include "console.hpp"

NC_LIBEXPORT(BOOL) ncEnableConsole()
{
	// Attempt to allocate console
	if(!AllocConsole())
		return 1;

	// Get the info / Set the scroll-buffer
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = 2000;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	// Redirect STDOUT
	long lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	int hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	FILE* fp = _fdopen(hConHandle, "w");
	*stdout = *fp;
	setvbuf(stdin, NULL, _IONBF, 0);

	// Redirect STDERR
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );

	// Sync with STDIO
	std::ios::sync_with_stdio();

	// Return
	return 0;
}