#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <conio.h>

#include "ncDriverDefines.h"

static NC_IMAGE_CONTAINER cont;
static NC_PROCESS_CONTAINER proc;

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


	// Image connection
	NC_CONNECT_INFO_R imgNcr;
	imgNcr.iDSLinkVersion = NC_DS_LINK_VERSION;
	imgNcr.iNCConnectInfoRSize = sizeof(NC_CONNECT_INFO_R);
	imgNcr.iNCImageContainerSize = sizeof(NC_IMAGE_CONTAINER);
	imgNcr.iNCImageEventSize = sizeof(NC_IMAGE_EVENT);
	imgNcr.iNCProcessContainerSize = sizeof(NC_PROCESS_CONTAINER);
	imgNcr.iNCProcessEventSize = sizeof(NC_PROCESS_EVENT);
	imgNcr.pBuff = (unsigned __int64)&cont;
	imgNcr.iSecurityCode = NC_LINK_SEC_CODE;
	//memcpy(&controlbuff, &imgNcr, sizeof(imgNcr));

	printf("Calling DeviceIoControl (image)\n");
	BOOL suc = DeviceIoControl(device,NC_CONNECTION_CODE_IMAGES,(LPVOID)&imgNcr,sizeof(NC_CONNECT_INFO_R),(LPVOID)&imgNcr,sizeof(NC_CONNECT_INFO_R),&dw,0);
	if(!suc) {
		printf("Could not dispatch driver link (image)! (%d)\n", GetLastError());
		system("pause");
		return 2;
	}


	//// Process connection
	//NC_CONNECT_INFO_R procNcr;
	//procNcr.iDSLinkVersion = NC_DS_LINK_VERSION;
	//procNcr.iNCConnectInfoRSize = sizeof(NC_CONNECT_INFO_R);
	//procNcr.iNCImageContainerSize = sizeof(NC_IMAGE_CONTAINER);
	//procNcr.iNCImageEventSize = sizeof(NC_IMAGE_EVENT);
	//procNcr.iNCProcessContainerSize = sizeof(NC_PROCESS_CONTAINER);
	//procNcr.iNCProcessEventSize = sizeof(NC_PROCESS_EVENT);
	//procNcr.pBuff = (unsigned __int64)&proc;
	//procNcr.iSecurityCode = NC_LINK_SEC_CODE;

	//printf("Calling DeviceIoControl (process)\n");
	//suc = DeviceIoControl(device,NC_CONNECTION_CODE_PROCESSES,(LPVOID)&procNcr,sizeof(NC_CONNECT_INFO_R),(LPVOID)&procNcr,sizeof(NC_CONNECT_INFO_R),&dw,0);
	//if(!suc) {
	//	printf("Could not dispatch driver link (process)! (%d)\n", GetLastError());
	//	system("pause");
	//	return 2;
	//}

	while(true)
	{
		printf("%d.", cont.iCount);
		if(cont.iCount > 0)
		{
			printf("\n");
			for(int i = 0; i < cont.iCount; i++)
				printf("Image (%d): %s\n", cont.oEvents[i].iPID, cont.oEvents[i].szImageName);
			cont.iCount = 0;
		}
		/*if(proc.iCount > 1)
		{
			printf("\n");
			for(int i = 0; i < proc.iCount; i++)
				printf("Process (%d <- %d)", proc.oEvents[i].iPID, proc.oEvents[i].iParentPID);
			proc.iCount = 0;
		}*/
		Sleep(1);
	}

	printf("Closing handle\n");
	CloseHandle(device);

	printf("Done.\n");

	system("pause");
	return 0;
}
