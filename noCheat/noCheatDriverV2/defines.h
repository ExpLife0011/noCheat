/************************************************************************/
/* Basic driver defines                                                 */
/************************************************************************/
#ifndef NCH_DEFINES__
#define NCH_DEFINES__

// Debug?
//	Comment out to set to release mode
//	(**THIS MUST BE DONE MANUALLY SINCE VS
//	DOES NOT SIGNAL TO THE BUILD ENVIRONMENTS!**)
#define DEBUG

// Setup Assertions
#ifdef DEBUG
#include <assert.h>
#define NASSERT(a, b) assert(a)
#else
#define NASSERT(a, b) if(!(a)) b;
#endif

// Ignore unused labels on debug (due to assertions)
#ifdef DEBUG
#pragma warning(disable:4102)
#endif

// Setup the log function
#ifdef DEBUG
#define LOG(a, ...) DbgPrint("[nC] " a "\n", __VA_ARGS__)
#define LOG2(a, ...) LOG("\t- " a, __VA_ARGS__)
#define LOG3(a, ...) LOG("\t\t- " a, __VA_ARGS__)
#else
#define LOG(...)
#define LOG2(...)
#define LOG3(...)
#endif

// Set up unicode processing method
#define MOVEANSI(vA, vB) UMoveAnsiString(&(vA), vB, sizeof(vA))

// Setup which callback we should use for process creation detection
#if (NTDDI_VERSION >= NTDDI_VISTASP1)
#define NC_PCN_EXTENDED
#define NC_PROCESSCREATE_NOTIFY(vA, vB) PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)&vA, vB);
#else
#define NC_PROCESSCREATE_NOTIFY(vA, vB) PsSetCreateProcessNotifyRoutine((PCREATE_PROCESS_NOTIFY_ROUTINE)&vA, vB);
#endif

#endif