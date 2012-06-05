#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <Wbemidl.h>
#include <comdef.h>
#include <tchar.h>
#include <WTypes.h>
#include <signal.h>
#include <TlHelp32.h>

#pragma comment(lib, "wbemuuid.lib")

#include <nc/ncCommon.hpp>

VOID onEngine(long pid)
{
	printf("---- START MODULE LIST ----\n");
	printf("Processing PID %d < Engine.exe > \n", pid);
	printf("- NOTE THAT THIS DOES NOT RISK YOUR -\n- COMBAT ARMS ACCOUNT! -\n\n");
	std::vector<MODULEENTRY32> mods;
	INT num = ncGetModuleSnapshot(&mods, pid);
	if(num < -1)
		printf("Could not get module snapshot!\n");
	else if(num == 0)
		printf("No modules!\n");
	else{
		std::vector<MODULEENTRY32>::iterator mit;
		for(mit=mods.begin(); mit < mods.end(); mit++)
		{
			char* asc = new char[wcslen((wchar_t*)((*mit).szModule)) + 1];
			wcstombs( asc, (wchar_t*)((*mit).szModule), wcslen((wchar_t*)((*mit).szModule)) );
			asc[wcslen((wchar_t*)((*mit).szModule))] = 0x00;
			printf("%s (%d <0x%08x>)\n", asc, (*mit).modBaseSize, (*mit).modBaseAddr);
		}
		printf("----- END MODULE LIST -----\n");
	}
}

VOID onStartup()
{
	printf("Successfully started up WMI listener.\n");
}

VOID onEvent(IWbemClassObject* procEvt)
{	
	ncWBEMResult res(procEvt);
	char buf[256] = {0};
	long pid;
	BOOL r = res.getString((char*)&buf, _T("Caption"));
	r = res.getLong(&pid, _T("ProcessId"));
	if(r)
		printf("Caption: %s, PID: %d\n", (char*)&buf, pid);
	else
		printf("Could not get caption!!!\n");
	if(strcmp(buf, "Engine.exe") == 0)
		onEngine(pid);
}


int CALLBACK WinMain(
HINSTANCE hInstance,
HINSTANCE hPrevInstance,
LPSTR lpCmdLine,
int nCmdShow
)
{
	// Test enabling the console
	ncEnableConsole();

	// Setup procmon
	ncPMCallbackInfo cbInfo;
	cbInfo.onEventHandler = (_ncPMEvent*)&onEvent;
	cbInfo.onShutdownHandler = NULL;
	cbInfo.onStartupHandler = (_ncPMStartup*)&onStartup;
	ncStartProcessListening(&cbInfo);

	// Sleep so we can make sure it's all working!
	while(true)
	{
		Sleep(30);
	}

	// Return
	return 0;
}
