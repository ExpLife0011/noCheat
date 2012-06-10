#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <conio.h>

/*
 * Connection info sent from the service
 */
struct NC_CONNECT_INFO
{
	unsigned long secCode; // The security code - currently EAT_STOOL
	unsigned long ILBuffAddr;	// The address to map to for image-loading events
	unsigned long ILBuffLen; // The buffer length (in bytes) for image-loading events
};

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
	unsigned char outputbuff[1024*32];
	DWORD controlbuff[64];
	DWORD dw;

	NC_CONNECT_INFO n;
	n.ILBuffAddr = (unsigned long)&outputbuff;
	n.ILBuffLen = 1024*32;
	n.secCode = 0xEA757001;

	memcpy(&controlbuff, &n, sizeof(NC_CONNECT_INFO));

	printf("Calling DeviceIoControl\n");
	DeviceIoControl(device,1000,controlbuff,256,controlbuff,256,&dw,0);

	unsigned char cnt = 0;

	while(cnt < 200)
	{
		if(cnt != outputbuff[0])
		{
			cnt = outputbuff[0];
			printf("Recvd (%d)\n", cnt);
		}
		Sleep(1);
	}

	printf("Closing handle\n");
	CloseHandle(device);

	printf("Done.\n");

	system("pause");
	return 0;
}