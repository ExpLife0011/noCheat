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
	
	DWORD dw;

	NC_CONNECT_INFO_INPUT inp;
	
	NC_CONNECT_INFO_OUTPUT returnInf;
	NC_PROCESS_CONTAINER proc;
	NC_IMAGE_CONTAINER image;
	
	inp.iDSLinkVersion = NC_DS_LINK_VERSION;
	inp.iSecurityCode = NC_LINK_SEC_CODE;
	inp.iImageContainerSize = sizeof(NC_IMAGE_CONTAINER);
	inp.iImageEventSize = sizeof(NC_IMAGE_EVENT);
	inp.iProcessContainerSize = sizeof(NC_PROCESS_CONTAINER);
	inp.iProcessEventSize = sizeof(NC_PROCESS_EVENT);
	inp.iReturnSize = sizeof(NC_CONNECT_INFO_OUTPUT);
	
	inp.pReturnInfo = (unsigned __int64) &returnInf;
	inp.pProcessCreateContainer = (unsigned __int64) &proc;
	inp.pImageLoadContainer = (unsigned __int64) &image;

	BOOL suc = DeviceIoControl(device, NC_CONNECTION_CODE, (LPVOID)&inp, sizeof(NC_CONNECT_INFO_INPUT), NULL, 0, &dw, 0);

	printf("Return code: %d{%d,%d,%d}\n", returnInf.bSuccess, returnInf.bBlocked, returnInf.bAccessDenied, returnInf.bSizeMismatch);

	while(true)
	{
		if(image.iCount > 0)
		{
			for(int i = 0; i < image.iCount; i++)
				printf("Image (%d): %s\n", image.oEvents[i].iPID, image.oEvents[i].szImageName);
			image.iCount = 0;
		}else Sleep(30000);
	}

	printf("Closing handle\n");
	CloseHandle(device);

	printf("Done.\n");

	system("pause");
	return 0;
}
