#ifndef #H_PROC_MODULELIST__
#define #H_PROC_MODULELIST__

#include <windows.h>
#include <vector>
#include <TlHelp32.h>

NC_LIBEXPORT(INT) ncGetModuleSnapshot(std::vector<MODULEENTRY32>* outVec, INT pid);

#endif