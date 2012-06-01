/*
 * noCheat Process Management Functionality
 */
#ifndef H_NC_COMMON_PROC__
#define H_NC_COMMON_PROC__

#include <windows.h>
#include <Wbemidl.h>
#include <vector>
#include <TlHelp32.h>

#pragma comment(lib, "wbemuuid.lib")

/*
 * Process Monitoring Event Handlers
 *	Use these with ncStartProcessListening to handle events
 *	called by WMI to indicate when a process is started.
 */
typedef VOID (*_ncPMEvent)(IWbemClassObject* procEvt);
typedef VOID (*_ncPMShutdown)();
typedef VOID (*_ncPMStartup)();

typedef struct {
	_ncPMEvent* onEventHandler;
	_ncPMShutdown* onShutdownHandler;
	_ncPMStartup* onStartupHandler;
} ncPMCallbackInfo;

extern "C" {
/*
 * ncStartProcessListening()
 *	Registers callbacks and starts listening for process
 *	creation. Called whenever a process is created.
 */
BOOL ncStartProcessListening(ncPMCallbackInfo* cbInfo);

/*
 * ncStopProcessListening()
 *	Stops/cleans up the process monitoring system
 */
VOID ncStopProcessListening();

/*
 * class ncWBEMResult;
 *	Use this class to extract data from the WMI events
 *	which return process execution data
 */
class ncWBEMResult{
public:
	ncWBEMResult(IWbemClassObject* wbem);
	~ncWBEMResult();

	BOOL getString(char* out, LPCWSTR name);
	BOOL getLong(long* out, LPCWSTR name);
protected:
	IWbemClassObject* _w;
	IWbemClassObject *pinstPkgStatus;
} ;

/*
 * ncGetModuleSnapshot()
 *	Gets the modules associated with a PID
 */
INT ncGetModuleSnapshot(std::vector<MODULEENTRY32>* outVec, INT pid);

}
#endif