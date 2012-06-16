/************************************************************************/
/* Structures used exclusively in the driver (non-exported)             */
/************************************************************************/
#ifndef NCH_STRUCTURES__
#define NCH_STRUCTURES__

// Include DDK
#include <ntddk.h>

// Include structures
#include "ncDriverDefines.h"

// Create map params
struct MAP_PARAMS
{
	unsigned char bMapped;
	unsigned __int32 iSize;
	void* oContainer;
	PMDL mdl;
};

// Create our mapped space table
#pragma pack (push,1)
struct NC_MAPPED_SPACE_TABLE
{
	unsigned char bLink;

	MAP_PARAMS Return, Images, Processes;
};
#pragma pack (pop)

#endif