#ifndef H_PROC_MON__
#define H_PROC_MON__

#define _WIN32_DCOM

#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>

#include "defines.hpp"

typedef VOID (*_ncPMEvent)(IWbemClassObject* procEvt);
typedef VOID (*_ncPMShutdown)();
typedef VOID (*_ncPMStartup)();

typedef struct {
	_ncPMEvent* onEventHandler;
	_ncPMShutdown* onShutdownHandler;
	_ncPMStartup* onStartupHandler;
} ncPMCallbackInfo;

NC_LIBEXPORT(BOOL) ncStartProcessListening(ncPMCallbackInfo* cbInfo);
NC_LIBEXPORT(VOID) ncStopProcessListening();

class ncPMEventHandler : public IWbemObjectSink
{
	LONG m_lRef;
	bool bDone;
	_ncPMShutdown* _onShutdownHandler;
	_ncPMEvent* _onEventHandler;

public:
	ncPMEventHandler(_ncPMShutdown* onShutdownHandler, _ncPMEvent* onEventHandler)
	{m_lRef = 0; _onShutdownHandler=onShutdownHandler; _onEventHandler=onEventHandler;}

	~ncPMEventHandler() { bDone = true; }

	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();        
	virtual HRESULT 
		STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);

	virtual HRESULT STDMETHODCALLTYPE Indicate( 
		LONG lObjectCount,
		IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray
		);

	virtual HRESULT STDMETHODCALLTYPE SetStatus(LONG lFlags, HRESULT hResult, BSTR strParam, IWbemClassObject __RPC_FAR *pObjParam);
};

#endif