/************************************************************************/
/* Structures used exclusively in the driver (non-exported)             */
/************************************************************************/
#ifndef NCH_STRUCTURES__
#define NCH_STRUCTURES__

// Include DDK
#include <ntddk.h>

// Include structures
#include "ncDriverDefines.h"

// Create struct macro
#define MAPPED_SPACE(name, type, lname) struct _MS_##name##{unsigned char bMapped; unsigned __int32 iSize; struct type* oContainer; } lname;

// Create our mapped space table
struct NC_MAPPED_SPACE_TABLE
{
	MAPPED_SPACE(RETURN, NC_CONNECT_INFO_OUTPUT, Return)
	MAPPED_SPACE(IMAGES, NC_IMAGE_CONTAINER, Images)
	MAPPED_SPACE(PROCESSES, NC_PROCESS_CONTAINER, Processes)
};

// Create temporary mapping parameters struct
//	This is used in the link system, and MUST
//	have the same structure of the struct
//	created by MAPPED_SPACE, less the pointer
struct TEMP_MAP_PARAMS 
{
	unsigned char bMapped;
	unsigned __int32 iSize;
	void* oContainer;
};

#endif