/************************************************************************/
/* Callbacks                                                            */
/*   These functions are called by the system after being registered    */
/************************************************************************/

// Include DDK
#include <ntddk.h>

// Include driver headers
#include "defines.h"
#include "structures.h"
#include "ncDriverDefines.h"
#include "utils.h"
#include "globals.h"

/*
 * Called when a process is created
 *	(EXTENDED)
 */
#ifdef NC_PCN_EXTENDED
extern "C" VOID ProcessCreateCallback(PEPROCESS Process, HANDLE ProcessId, PPS_CREATE_NOTIFY_INFO CreateInfo)
{
	// Setup vars
	struct NC_PROCESS_EVENT pe;
	struct NC_PROCESS_CONTAINER* pProcessEvents;

	// Setup pointer
	pProcessEvents = (struct NC_PROCESS_CONTAINER*)sSpaces.Processes.oContainer;

	// Check to see if there is a link and return if there is not
	if(sSpaces.Processes.bMapped == 0) return;

	// Check for overflow
	if(pProcessEvents->iCount >= NC_EVENT_BACKLOG)
	{
		// Log and return
		LOG2("Reached process creation event backlog limit!");
		return;
	}

	// Set up new process object
	pe.bExtended = 1;
	pe.iPID = (unsigned __int32)ProcessId;
	pe.iCallingThread.iUniqueProcess = (unsigned __int32)CreateInfo->CreatingThreadId.UniqueProcess;
	pe.iCallingThread.iUniqueThread = (unsigned __int32)CreateInfo->CreatingThreadId.UniqueThread;
	pe.iParentPID = (unsigned __int32)CreateInfo->ParentProcessId;
	
	// Get Process File Name
	MOVEANSI(pe.szProcessFileName, (PUNICODE_STRING)CreateInfo->ImageFileName);

	// Get Command Line
	MOVEANSI(pe.szCommandLine, (PUNICODE_STRING)CreateInfo->CommandLine);

	// Get Image Name
	MOVEANSI(pe.szFileName, &CreateInfo->FileObject->FileName);

	// Assign to memory
	pProcessEvents->oEvents[pProcessEvents->iCount] = pe;

	// Increment count
	pProcessEvents->iCount = pProcessEvents->iCount + 1;

	// Log
	LOG("ProcessEX (%d <- %d)", pe.iPID, pe.iParentPID);
}

#endif

/*
 * Called when a process is created
 *	(BASIC / PREVISTA)
 */
#ifndef NC_PCN_EXTENDED
extern "C" VOID ProcessCreateCallback(HANDLE ParentId, HANDLE ProcessId, BOOLEAN Create)
{
	
	// Setup vars
	struct NC_PROCESS_EVENT pe;
	struct NC_PROCESS_CONTAINER* pProcessEvents;

	// Setup pointer
	pProcessEvents = (NC_PROCESS_CONTAINER*)sSpaces.Processes.oContainer;

	// Check to see if the process is being destroyed and return if so
	if(!Create) return;

	// Check to see if there is a link and return if there is not
	if(sSpaces.Processes.bMapped == 0) return;

	// Check for overflow
	if(pProcessEvents->iCount >= NC_EVENT_BACKLOG)
	{
		// Log and return
		LOG2("Reached process creation event backlog limit!");
		return;
	}

	// Set up new process object
	pe.bExtended = 0;
	pe.iPID = (unsigned __int32)ProcessId;
	pe.iParentPID = (unsigned __int32)ParentId;

	// Assign to memory
	pProcessEvents->oEvents[pProcessEvents->iCount] = pe;

	// Increment count
	pProcessEvents->iCount = pProcessEvents->iCount + 1;

	// Print
	LOG("Process (%d <- %d)", ProcessId, ParentId);
}

#endif

/*
 * Called whenever an image (DLL or EXE) is loaded
 */
extern "C" VOID ImageLoadCallback(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo)
{
	// Setup vars
	struct NC_IMAGE_EVENT ie;
	unsigned int offset;

	// Check to see there is a link and return if there is not
	if(sSpaces.Images.bMapped == 0) return;

	// Setup pointer
	NC_IMAGE_CONTAINER* oContainer = (NC_IMAGE_CONTAINER*) sSpaces.Images.oContainer;

	// Check for overflow
	if(oContainer->iCount >= NC_EVENT_BACKLOG)
	{
		// Log and return
		LOG2("Reached image event backlog limit!");
		return;
	}

	// Set up a new image object
	ie.iPID = (unsigned __int32)ProcessId;
	ie.bKernelLand = (unsigned char)ImageInfo->SystemModeImage;
	ie.iImageBase = (unsigned __int64)ImageInfo->ImageBase;
	ie.iSize = (unsigned __int32)ImageInfo->ImageSize;

	// Get Image name
	MOVEANSI(ie.szImageName, FullImageName);

	// Assign to memory
	oContainer->oEvents[oContainer->iCount] = ie;

	// Increment count
	oContainer->iCount = oContainer->iCount + 1;

	// Log
	LOG3("Image (%d): %wZ", ProcessId, FullImageName);
}

/*
 * Called when a thread is created
 *	or destroyed
 */
extern "C" VOID ThreadCreateCallback(HANDLE ProcessId, HANDLE ThreadId, BOOLEAN Create)
{
	// Check to see if it's a creation
	if(!Create) return;

	// Check to see if threads are mapped
	if(sSpaces.Threads.bMapped == 0) return;

	// Setup pointer
	NC_THREAD_CONTAINER* oContainer = (NC_THREAD_CONTAINER*)sSpaces.Threads.oContainer;

	// Create new thread event
	NC_THREAD_EVENT te;

	// Populate event
	te.iPID = (unsigned __int32) ProcessId;
	te.iThreadId = (unsigned __int32) ThreadId;

	// Assign event
	oContainer->oEvents[oContainer->iCount] = te;

	// Increment amount
	oContainer->iCount += 1;

	// Log
	LOG("Thread %d <- %d", ThreadId, ProcessId);
};