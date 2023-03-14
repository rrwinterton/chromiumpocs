#pragma once
#include <windows.h>
#include <stdint.h>

//export/import functions from dll
#ifdef _DLLLOAD_DLL_EXPORT
#define DLLLOAD_DLL_API __declspec(dllexport)
#else
#define DLLLOAD_DLL_API __declspec(dllimport)
#endif

//exter name to remove name mangle
#ifdef __cplusplus 
#define EXTERN_C extern "C"
#endif

//extern and export
EXTERN_C DLLLOAD_DLL_API uint32_t addInt(uint32_t a, uint32_t b);
//make it an easy type to load
typedef uint32_t (*addIntPtr)(uint32_t, uint32_t);