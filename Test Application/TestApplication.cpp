#include <windows.h>
#include <iostream>
#include <stdio.h>

#include <nc/ncCommon.hpp>

int CALLBACK WinMain(
HINSTANCE hInstance,
HINSTANCE hPrevInstance,
LPSTR lpCmdLine,
int nCmdShow
)
{
	// Test enabling the console
	ncEnableConsole();

	// Write something
	unsigned char ot[2] = {0x49, 0x00};
	unsigned int a = 999;
	rol8(&ot[0], 1);
	rol32(&a, 1);
	std::cout << "Turned I into " << &ot[0] << std::endl;
	std::cout << "Turned 999 into " << a << std::endl;
	
	// Sleep so we can make sure it's all working!
	Sleep(3000);

	// Return
	return 0;
}