#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <conio.h>

#include "ncDriverDefines.h"

//static NC_IMAGE_CONTAINER cont;
//static NC_PROCESS_CONTAINER proc;

int main()
{
	//open device
	/*HANDLE device = CreateFile("\\\\.\\NOCHEAT",GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_SYSTEM,0);
	
	if(device == INVALID_HANDLE_VALUE)
	{
		printf("Could not connect to driver service! (%d)\n", GetLastError());
		system("pause");
		return 1;
	}*/
	
	DWORD dw;

	//suc = DeviceIoControl(device,NC_CONNECTION_CODE_PROCESSES,(LPVOID)&procNcr,sizeof(NC_CONNECT_INFO_INPUT),(LPVOID)&result,sizeof(NC_CONNECT_INFO_OUTPUT),&dw,0);


	/*printf("Closing handle\n");
	CloseHandle(device);*/

	printf("Done.\n");

	system("pause");
	return 0;
}
