#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <conio.h>

#include "ncDriverDefines.h"

NC_IMAGE_CONTAINER cont;

VOID WaitForDriver()
{
	while(cont.bDriverWriting == 1){}
}

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
	DWORD dw;

	NC_CONNECT_INFO_R ncr;
	ncr.iDSLinkVersion = NC_DS_LINK_VERSION;
	ncr.iNCConnectInfoRSize = sizeof(NC_CONNECT_INFO_R);
	ncr.iNCImageContainerSize = sizeof(NC_IMAGE_CONTAINER);
	ncr.iNCImageEventSize = sizeof(NC_IMAGE_EVENT);
	ncr.pBuff = (__int64)&cont;
	ncr.iSecurityCode = NC_LINK_SEC_CODE;
	memcpy(&controlbuff, &ncr, sizeof(ncr));

	printf("Calling DeviceIoControl\n");
	BOOL suc = DeviceIoControl(device,NC_CONNECTION_CODE_IMAGES,controlbuff,256,controlbuff,256,&dw,0);
	if(!suc) {
		printf("Could not dispatch driver link! (%d)\n", GetLastError());
		system("pause");
		return 2;
	}

	while(true)
	{

		//printf("%d.", cont.iCount);

		//WaitForDriver();
		//cont.bServiceWriting = 1;
		if(cont.iCount > 0)
		{
			//printf("\n");
			for(int i = 0; i < cont.iCount; i++)
			{
				printf("Image (%d): %s\n", cont.oEvents[i].iPID, cont.oEvents[i].szImageName);
			}
			cont.iCount = 0;
		}else
			Sleep(1);
		//cont.bServiceWriting = 0;
	}

	printf("Closing handle\n");
	CloseHandle(device);

	printf("Done.\n");

	system("pause");
	return 0;
}
