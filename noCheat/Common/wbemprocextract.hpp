#ifndef H_PROC_WBEMSTRUCT__
#define H_PROC_WBEMSTRUCT__

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

#endif