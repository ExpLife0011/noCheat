#include <windows.h>

int WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
{
	MessageBoxW(NULL, L"Hello!", L"Hello!", MB_OK);

	return 0;
}