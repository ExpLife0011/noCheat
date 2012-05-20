#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <Wbemidl.h>
#include <comdef.h>
#include <tchar.h>
#include <WTypes.h>

#pragma comment(lib, "wbemuuid.lib")

#include <nc/ncCommon.hpp>

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