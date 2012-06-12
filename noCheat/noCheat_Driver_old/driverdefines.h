/************************************************************************/
/* Driver Structs                                                       */
/*  Used to define the various structures of the buffer link            */
/************************************************************************/

/*
 * Connection info sent from the service
 */
struct NC_CONNECT_INFO
{
	unsigned long secCode; // The security code - currently EAT_STOOL
	unsigned long ILBuffAddr;	// The address to map to for image-loading events
	unsigned long ILBuffLen; // The buffer length (in bytes) for image-loading events
};

/*
 * Holds information about the buffer totals
 *	and event sizes and such
 */
struct NC_EVENT_SETTINGS
{
	unsigned long buffSize;
	unsigned long maxEvents;
	unsigned char* buff;
};

/*
 *	Manual definition of the struct for image info
 */
#ifndef _NC_DRIVER
typedef struct _IMAGE_INFO {
	union {
		ULONG  Properties;
		struct {
			ULONG ImageAddressingMode  : 8; //code addressing mode
			ULONG SystemModeImage      : 1; //system mode image
			ULONG ImageMappedToAllPids : 1; //mapped in all processes
			ULONG Reserved             : 22;
		};
	};
	PVOID  ImageBase;
	ULONG  ImageSelector;
	ULONG  ImageSize;
	ULONG  ImageSectionNumber;
} IMAGE_INFO;
#endif

/*
 * Our condensed image load struct
 *	that is passed back to the user-land service
 */
struct NC_IL_INFO
{
	char puszFullImageName[260]; // MAX_PATH, which is not available in the DDK
	HANDLE hwndProcessId;
	IMAGE_INFO pinfImageInfo;
};

/*
 * Values placed at the beginning of the IL stream
 *	This is used to signal writing, etc.
 */
struct NC_IL_HEAD
{
	unsigned char count; // The count of objects
	union
	{
		unsigned char Flags;
		struct 
		{
			unsigned char dwrite : 1; // Whether or not the driver is writing
			unsigned char swrite : 1; // Whether or not the service is writing
			unsigned char cReserved : 6;
		};
	};
};

/* 
 * EAT_STOOL security identifier
 */
#define EAT_STOOL 0xEA757001