#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <conio.h>

#include "ncDriverDefines.h"

NC_IMAGE_CONTAINER cont;
NC_PROCESS_CONTAINER proc;

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

	// Image connection
	ncr.iDSLinkVersion = NC_DS_LINK_VERSION;
	ncr.iNCConnectInfoRSize = sizeof(NC_CONNECT_INFO_R);
	ncr.iNCImageContainerSize = sizeof(NC_IMAGE_CONTAINER);
	ncr.iNCImageEventSize = sizeof(NC_IMAGE_EVENT);
	ncr.iNCProcessContainerSize = sizeof(NC_PROCESS_CONTAINER);
	ncr.iNCProcessEventSize = sizeof(NC_PROCESS_EVENT);
	ncr.pBuff = (unsigned __int64)&cont;
	ncr.iSecurityCode = NC_LINK_SEC_CODE;
	memcpy(&controlbuff, &ncr, sizeof(ncr));

	printf("Calling DeviceIoControl (image)\n");
	BOOL suc = DeviceIoControl(device,NC_CONNECTION_CODE_IMAGES,controlbuff,256,controlbuff,256,&dw,0);
	if(!suc) {
		printf("Could not dispatch driver link (image)! (%d)\n", GetLastError());
		system("pause");
		return 2;
	}

	// Process connection
	ncr.pBuff = (unsigned __int64)&proc;
	memcpy(&controlbuff, &ncr, sizeof(ncr));

	printf("Calling DeviceIoControl (process)\n");
	suc = DeviceIoControl(device,NC_CONNECTION_CODE_PROCESSES,controlbuff,256,controlbuff,256,&dw,0);
	if(!suc) {
		printf("Could not dispatch driver link (process)! (%d)\n", GetLastError());
		system("pause");
		return 2;
	}

	while(true)
	{
		if(cont.iCount > 0)
		{
			for(int i = 0; i < cont.iCount; i++)
				printf("Image (%d): %s\n", cont.oEvents[i].iPID, cont.oEvents[i].szImageName);
			cont.iCount = 0;
		}
		if(proc.iCount > 1)
		{
			for(int i = 0; i < proc.iCount; i++)
				printf("Process (%d <- %d)", proc.oEvents[i].iPID, proc.oEvents[i].iParentPID);
			proc.iCount = 0;
		}
		Sleep(1);
	}

	printf("Closing handle\n");
	CloseHandle(device);

	printf("Done.\n");

	system("pause");
	return 0;
}
