#include <windows.h>
#include <iostream>

#include <nc/ncUtil.hpp>

HMODULE gInstance;
BOOL ran = FALSE;

void OnInject()
{
	// Enable the console
	ncEnableConsole();
	
	// Print a success message
	printf_s("Successfully started noCheat!");
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if(ulReason == DLL_PROCESS_ATTACH)
	{
		gInstance = hModule;
		DisableThreadLibraryCalls(hModule);
		if(!ran)
		{
			ran = TRUE;
			OnInject();
		}
	}

	return TRUE;
}