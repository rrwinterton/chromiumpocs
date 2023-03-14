#include <windows.h>
#include <stdint.h>

#include "dllloaddll.h"

//extern "C" {

EXTERN_C DLLLOAD_DLL_API uint32_t addInt(uint32_t a, uint32_t b) {
		uint32_t c;
		c = a + b;
		return c;
	}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}
