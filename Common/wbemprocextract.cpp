#include <windows.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <tchar.h>

#include "defines.hpp"
#include "wbemprocextract.hpp"

ncWBEMResult::ncWBEMResult(IWbemClassObject *wbem)
{
	_w = wbem;
	_variant_t vTarget;
	pinstPkgStatus = NULL;
	HRESULT hres = wbem->Get(_T("TargetInstance"), 0, &vTarget, NULL, NULL);
	if(FAILED(hres))
		return;
	IUnknown* plUnknown = (IUnknown*)vTarget;
	plUnknown->QueryInterface(IID_IWbemClassObject, (void**)&pinstPkgStatus);
	plUnknown->Release();	
}

BOOL ncWBEMResult::getString(char* out, LPCWSTR name)
{
	if(pinstPkgStatus == NULL)
		return FALSE;
	_variant_t v;
	HRESULT hres = pinstPkgStatus->Get(name, 0, &v, NULL, NULL);
	if(FAILED(hres))
		return FALSE;
	if(V_VT(&v) != VT_BSTR)
		return FALSE;
	const _bstr_t l_string(v);
	const char * p(l_string);
	memcpy(out, p, strlen(p));
	
	return TRUE;
}

BOOL ncWBEMResult::getLong(long* out, LPCWSTR name)
{
	if(pinstPkgStatus == NULL)
		return FALSE;
	_variant_t v;
	HRESULT hres = pinstPkgStatus->Get(name, 0, &v, NULL, NULL);
	if(FAILED(hres))
		return FALSE;
	*out = (long)v;
	return TRUE;
}

ncWBEMResult::~ncWBEMResult()
{
	if(pinstPkgStatus != NULL)pinstPkgStatus->Release();
	pinstPkgStatus = NULL;
}