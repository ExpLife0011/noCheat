#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <conio.h>

#include "driverdefines.h"

// Setup output buff
unsigned char outputbuff[1024*32];

/*
 * Waits for the driver to be finished writing
 */
inline void waitForDriver()
{
	NC_IL_HEAD* head = (NC_IL_HEAD*)outputbuff;
	while(head->dwrite == 1);
}

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

	NC_CONNECT_INFO n;
	n.ILBuffAddr = (unsigned long)&outputbuff;
	n.ILBuffLen = 1024*32;
	n.secCode = EAT_STOOL;

	memcpy(&controlbuff, &n, sizeof(NC_CONNECT_INFO));

	printf("Calling DeviceIoControl\n");
	BOOL suc = DeviceIoControl(device,1000,controlbuff,256,controlbuff,256,&dw,0);
	if(!suc) {
		printf("Could not dispatch driver link!\n");
		system("pause");
		return 2;
	}

	NC_IL_HEAD* head = (NC_IL_HEAD*)outputbuff;

	while(true)
	{
		waitForDriver();
		head->swrite = 1;
		if(outputbuff[0] > 0)
		{
			
			for(unsigned char i = 0; i < outputbuff[0]; i++)
			{
				NC_IL_INFO* inf = (NC_IL_INFO*)((int)outputbuff + sizeof(NC_IL_HEAD) + (i * sizeof(struct NC_IL_INFO)));
				printf("Recvd: (%d) %s\n", inf->hwndProcessId, (char*)(inf->puszFullImageName));
			}
			outputbuff[0] = 0;
		}
		head->swrite = 0;
		Sleep(1);
	}

	printf("Closing handle\n");
	CloseHandle(device);

	printf("Done.\n");

	system("pause");
	return 0;
}