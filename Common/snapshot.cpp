#include <windows.h>
#include <vector>
#include <TlHelp32.h>

#include "defines.hpp"


NC_LIBEXPORT(INT) ncGetModuleSnapshot(std::vector<MODULEENTRY32>* outVec, INT pid)
{
	// Get modules
	HANDLE ct32s = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, (DWORD)pid);
	
	// Create first module entry and pass it to first
	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);
	BOOL ret = Module32First(ct32s, &me32);
	
	// Check initial return
	if(!ret && GetLastError() == ERROR_NO_MORE_FILES)
		return -1;
	do
	{
		// Push it
		if(ret)
			outVec->push_back(me32);
		
		// Call next
		ret = Module32Next(ct32s, &me32);
	}while(ret && GetLastError() != ERROR_NO_MORE_FILES);

	// Return
	return (INT)outVec->size();
}