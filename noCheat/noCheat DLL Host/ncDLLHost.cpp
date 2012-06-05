#include <windows.h>


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if(fdwReason == DLL_PROCESS_ATTACH)
		Beep(1500, 50);

	// Return true
	return TRUE;
}