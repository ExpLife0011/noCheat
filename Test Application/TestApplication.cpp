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
	printf_s("This is a %s\n", "test!");
	std::cout << "This is also a test!\n";
	std::cerr << "This is an error message!\n";
	
	// Sleep so we can make sure it's all working!
	Sleep(3000);

	// Return
	return 0;
}