/************************************************************************/
/* Callbacks header file                                                */
/************************************************************************/
#ifndef NCH_CALLBACKS__
#define NCH_CALLBACKS__

// Include DDK
#include <ntddk.h>

/*
 * Called when a process is created
 *	(EXTENDED)
 */
#ifdef NC_PCN_EXTENDED
extern "C" VOID ProcessCreateCallback(PEPROCESS Process, HANDLE ProcessId, PPS_CREATE_NOTIFY_INFO CreateInfo);
#endif

/*
 * Called when a process is created
 *	(BASIC / PREVISTA)
 */
#ifndef NC_PCN_EXTENDED
extern "C" VOID ProcessCreateCallback(HANDLE ParentId, HANDLE ProcessId, BOOLEAN Create);
#endif

/*
 * Called whenever an image (DLL or EXE) is loaded
 */
extern "C" VOID ImageLoadCallback(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo);

/*
 * Called when a bugcheck is triggered
 */
extern "C" VOID BugCheckCallback(PVOID buffer, ULONG length);

#endif