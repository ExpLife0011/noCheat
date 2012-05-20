#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <Wbemidl.h>
#include <comdef.h>
#include <tchar.h>
#include <WTypes.h>

#pragma comment(lib, "wbemuuid.lib")

#include <nc/ncCommon.hpp>

VOID onEvent(IWbemClassObject* procEvt)
{
	BSTR bs = SysAllocString(_T(""));
	HRESULT hres = procEvt->GetObjectText(0, &bs);
	if(SUCCEEDED(hres))
	{
		_bstr_t tmp(bs, FALSE);
		const char* ms = static_cast<const char *>(tmp);
		printf("Process created: %s\n",ms);
	}else
		printf("Received event, but failed to get object serialization\n");
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
	cbInfo.onStartupHandler = NULL;
	ncStartProcessListening(&cbInfo);


	// Sleep so we can make sure it's all working!
	while(true)
	{
		Sleep(30);
	}

	// Return
	return 0;
}