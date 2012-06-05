#include <windows.h>
#include <iostream>
#include <tchar.h>

int main()
{
	printf("Signaling to hook MessageBoxW\n");

	//open device
	HANDLE device = CreateFile("\\\\.\\NOCHEAT",GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_SYSTEM,0);
	
	if(device == INVALID_HANDLE_VALUE)
	{
		printf("Could not connect to driver service!\n");
		system("pause");
		return 1;
	}
	char outputbuff[256];
	DWORD controlbuff[64];
	DWORD dw;

	// get index of NtCreateSection, and pass it to the driver, along with the
	//address of output buffer
	printf("Getting address of MessageBoxW\n");

	DWORD * addr = (DWORD *)(1+(DWORD)GetProcAddress(LoadLibrary("User32.dll"),"MessageBoxW"));
	printf("Zeroing memory...\n");
	ZeroMemory(outputbuff,256);
	printf("Setting controlbuff 0\n");
	controlbuff[0]=(*addr);
	printf("Setting controlbuff 1\n");
	controlbuff[1]=(DWORD)&outputbuff[0];
	printf("Calling DeviceIoControl\n");
	DeviceIoControl(device,1000,controlbuff,256,controlbuff,256,&dw,0);
	printf("Closing handle\n");
	CloseHandle(device);

	printf("Done.\n");

	system("pause");
	return 0;
}