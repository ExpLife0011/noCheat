#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <conio.h>

#include "ncDriverDefines.h"

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
	
	UCHAR controlbuff[256];
	NC_IMAGE_CONTAINER cont;
	DWORD dw;

	NC_CONNECT_INFO_R ncr;
	ncr.iDSLinkVersion = NC_DS_LINK_VERSION;
	ncr.iNCConnectInfoRSize = sizeof(NC_CONNECT_INFO_R);
	ncr.iNCImageContainerSize = sizeof(NC_IMAGE_CONTAINER);
	ncr.iNCImageEventSize = sizeof(NC_IMAGE_EVENT);
	ncr.pBuff = (int)&cont;
	ncr.iSecurityCode = NC_LINK_SEC_CODE;
	memcpy(&controlbuff, &ncr, sizeof(ncr));

	printf("Calling DeviceIoControl\n");
	BOOL suc = DeviceIoControl(device,NC_CONNECTION_CODE_IMAGES,controlbuff,256,controlbuff,256,&dw,0);
	if(!suc) {
		printf("Could not dispatch driver link! (%d)\n", GetLastError());
		system("pause");
		return 2;
	}

	while(cont.iCount != 1)
	{
		Sleep(10);
	}

	printf("Closing handle\n");
	CloseHandle(device);

	printf("Done.\n");

	system("pause");
	return 0;
}
