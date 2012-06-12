#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <conio.h>


int main()
{
	//open device
	HANDLE device = CreateFile("\\\\.\\NOCHEAT",GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_SYSTEM,0);
	
	if(device == INVALID_HANDLE_VALUE)
	{
		printf("Could not connect to driver service!\n");
		system("pause");
		return 1;
	}
	
	DWORD controlbuff[64];
	DWORD dw;

	printf("Calling DeviceIoControl\n");
	BOOL suc = DeviceIoControl(device,1000,controlbuff,256,controlbuff,256,&dw,0);
	if(!suc) {
		printf("Could not dispatch driver link!\n");
		system("pause");
		return 2;
	}

	printf("Closing handle\n");
	CloseHandle(device);

	printf("Done.\n");

	system("pause");
	return 0;
}