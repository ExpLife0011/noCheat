#include <windows.h>
#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>

#include "processmon.hpp"
#include "defines.hpp"

#define NC_RIF(a) if(FAILED(a)) return FALSE;

static IWbemLocator *pLoc;
static IWbemServices *pSvc;
static IUnsecuredApartment* pUnsecApp;
static IUnknown* pStubUnk;
static ncPMEventHandler* pSink;
static IWbemObjectSink* pStubSink;

NC_LIBEXPORT(BOOL) ncStartProcessListening(ncPMCallbackInfo* cbInfo)
{

	// Init com library
	HRESULT hres;
	hres =  CoInitializeEx(0, COINIT_MULTITHREADED);
	NC_RIF(hres);

	// Initialize Security
	hres =  CoInitializeSecurity(
        NULL, 
        -1,
        NULL,
        NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, 
        EOAC_NONE, 
        NULL
        );
	NC_RIF(hres);

	// Locate initial locator to WMI
	pLoc = NULL;
    hres = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &pLoc);
	NC_RIF(hres);

	// Connect to WMI
	pSvc = NULL;
    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"), 
        NULL,
        NULL, 
        0, 
        NULL, 
        0, 
        0, 
        &pSvc
    );
	NC_RIF(hres);

	// Call startup callback
	if(cbInfo->onStartupHandler != NULL) ((_ncPMStartup)(cbInfo->onStartupHandler))();

	// Set security set proxy security
	hres = CoSetProxyBlanket(
        pSvc,
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        NULL,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE
    );
	NC_RIF(hres);

	// Setup to receive events
	pUnsecApp = NULL;

    hres = CoCreateInstance(CLSID_UnsecuredApartment, NULL, 
        CLSCTX_LOCAL_SERVER, IID_IUnsecuredApartment, 
        (void**)&pUnsecApp);
 
    pSink = new ncPMEventHandler(cbInfo->onShutdownHandler, cbInfo->onEventHandler);
    pSink->AddRef();

    pStubUnk = NULL; 
    pUnsecApp->CreateObjectStub(pSink, &pStubUnk);

    pStubSink = NULL;
    pStubUnk->QueryInterface(IID_IWbemObjectSink,
        (void **) &pStubSink);

    hres = pSvc->ExecNotificationQueryAsync(
        _bstr_t("WQL"), 
        _bstr_t("SELECT * " 
            "FROM __InstanceCreationEvent WITHIN 1 "
            "WHERE TargetInstance ISA 'Win32_Process'"), 
        WBEM_FLAG_SEND_STATUS, 
        NULL, 
        pStubSink);
	NC_RIF(hres);

	return TRUE;
}

NC_LIBEXPORT(VOID) ncStopProcessListening()
{
	// Cancel Async requests, if any
	pSvc->CancelAsyncCall(pStubSink);

	// Release all resources
	pSvc->Release();
    pLoc->Release();
    pUnsecApp->Release();
    pStubUnk->Release();
    pSink->Release();
    pStubSink->Release();
    CoUninitialize();
}

ULONG ncPMEventHandler::AddRef()
{
    return InterlockedIncrement(&m_lRef);
}

ULONG ncPMEventHandler::Release()
{
    LONG lRef = InterlockedDecrement(&m_lRef);
    if(lRef == 0)
        delete this;
    return lRef;
}

HRESULT ncPMEventHandler::QueryInterface(REFIID riid, void** ppv)
{
    if (riid == IID_IUnknown || riid == IID_IWbemObjectSink)
    {
        *ppv = (IWbemObjectSink *) this;
        AddRef();
        return WBEM_S_NO_ERROR;
    }
    else return E_NOINTERFACE;
}


HRESULT ncPMEventHandler::Indicate(long lObjectCount,
    IWbemClassObject **apObjArray)
{
	HRESULT hres = S_OK;

    for (int i = 0; i < lObjectCount; i++)
    {
		// Call callback
		if(_onEventHandler != NULL) ((_ncPMEvent)_onEventHandler)(apObjArray[i]);
    }

    return WBEM_S_NO_ERROR;
}

HRESULT ncPMEventHandler::SetStatus(
             LONG lFlags,
             HRESULT hResult,
             BSTR strParam,
             IWbemClassObject __RPC_FAR *pObjParam
        )
{
	
    if(lFlags == WBEM_STATUS_COMPLETE)
    {
        // Call callback
		if(_onShutdownHandler != NULL) ((_ncPMShutdown)_onShutdownHandler)();
    }

    return WBEM_S_NO_ERROR;
} 