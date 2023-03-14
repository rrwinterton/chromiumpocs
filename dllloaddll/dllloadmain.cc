#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

#include "dllloaddll.h"

//global (doesn't have to be)
HMODULE dllloaddll = NULL;

//initialize
uint32_t initialize(addIntPtr &pAddInt) {
	int Ret;
	pAddInt = NULL;
	//load dynamic library
	dllloaddll = LoadLibrary(L"dllloaddll.dll");
	Ret = GetLastError();
	if (Ret != ERROR_SUCCESS) {
		Ret = -1;
		goto GracefulExit;
	}
	//get access to function
	pAddInt = (addIntPtr)GetProcAddress(dllloaddll, "addInt");
	if (pAddInt == NULL) {
		Ret = -2;
		goto GracefulExit;
	}
	Ret = 0;
GracefulExit:
	return Ret;
}

//main
int main() {
	int Ret;

	uint32_t c;
	addIntPtr pAddInt = NULL;

	//get dll and function access
	Ret = initialize(pAddInt);
	if (Ret != 0) {		
		goto GracefulExit;
	}

	//call function
	c = pAddInt(1, 2);
	std::cout << "value of c: " << c << " error code: " << Ret << std::endl;

GracefulExit:
	//if dll was loaded free dll
	if (dllloaddll) {
		FreeLibrary(dllloaddll);
	}
	return Ret;

}
