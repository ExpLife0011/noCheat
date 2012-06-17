/************************************************************************/
/* Global variables                                                     */
/************************************************************************/
#ifndef NCH_GLOBALS__
#define NCH_GLOBALS__

// Include DDK
#include <ntddk.h>

// Include driver headers
#include "structures.h"

// Declare our link constants
static const WCHAR devicename[] = L"\\Device\\noCheat";
static const WCHAR devicelink[] = L"\\DosDevices\\NOCHEAT";

// Setup mapped spaces
extern "C" NC_MAPPED_SPACE_TABLE sSpaces;


#endif