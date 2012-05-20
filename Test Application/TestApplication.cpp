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

	// Test 1 Encryption
	unsigned char key[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
	unsigned char string[] = {'H', 'e', 'l', 'l', 'o', '\0'};
	ncEncrypt_1((unsigned char*)&string, 5, &key[0], 6);
	std::cout << "Encrypted string: " << (unsigned char*)&string << std::endl;
	ncDecrypt_1((unsigned char*)&string, 5, &key[0], 6);
	std::cout << "Decrypted string: " << (unsigned char*)&string;

	// Sleep so we can make sure it's all working!
	Sleep(3000);

	// Return
	return 0;
}