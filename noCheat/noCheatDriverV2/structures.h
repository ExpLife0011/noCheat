/************************************************************************/
/* Structures used exclusively in the driver (non-exported)             */
/************************************************************************/
#ifndef NCH_STRUCTURES__
#define NCH_STRUCTURES__

// Include DDK
#include <ntddk.h>

// Create our mapped space structs
struct NC_MAPPED_SPACE
{
	void* pAddr;
	unsigned char bMapped;
	unsigned __int32 iSize;
};
struct NC_MAPPED_SPACE_TABLE
{
	struct NC_MAPPED_SPACE Process;
	struct NC_MAPPED_SPACE Image;
};

#endif