#include <windows.h>
#include <iostream>
#include <tchar.h>

int main()
{
	printf("Signaling to hook MessageBoxW\n");

	//open device
	HANDLE device = CreateFile("\\\\.\\PROTECTOR",GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_SYSTEM,0);
	
	if(device == INVALID_HANDLE_VALUE)
	{
		printf("Could not connect to driver service!\n");
		system("pause");
		return 1;
	}
	char outputbuff[256];
	DWORD controlbuff[64];DWORD dw;

	// get index of NtCreateSection, and pass it to the driver, along with the
	//address of output buffer
	DWORD * addr = (DWORD *)(1+(DWORD)GetProcAddress(GetModuleHandle("User32.dll"),"MessageBoxW"));
	ZeroMemory(outputbuff,256);
	controlbuff[0]=addr[0];
	controlbuff[1]=(DWORD)&outputbuff[0];
	DeviceIoControl(device,1000,controlbuff,256,controlbuff,256,&dw,0);
	CloseHandle(device);

	printf("Done.\n");

	system("pause");
	return 0;
}