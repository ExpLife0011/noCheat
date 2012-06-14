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
	HANDLE device = CreateFile("\\\\.\\NOCHEAT",GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_SYSTEM,0);
	
	if(device == INVALID_HANDLE_VALUE)
	{
		printf("Could not connect to driver service! (%d)\n", GetLastError());
		system("pause");
		return 1;
	}
	
	DWORD dw;

	NC_CONNECT_INFO_INPUT inp;
	
	inp.iDSLinkVersion = NC_DS_LINK_VERSION;
	inp.iSecurityCode = NC_LINK_SEC_CODE;
	inp.iImageContainerSize = sizeof(NC_IMAGE_CONTAINER);
	inp.iImageEventSize = sizeof(NC_IMAGE_EVENT);

	BOOL suc = DeviceIoControl(device, NC_CONNECTION_CODE, (LPVOID)&inp, sizeof(NC_CONNECT_INFO_INPUT), NULL, 0, &dw, 0);


	printf("Closing handle\n");
	CloseHandle(device);

	printf("Done.\n");

	system("pause");
	return 0;
}
