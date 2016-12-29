//
// HotReload
//

#pragma once

#include <string>

#ifdef HOTRELOAD_EXPORTS
#define HOTRELOAD_API __declspec(dllexport)
#else
#define HOTRELOAD_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "../x64/Release/HotReload.lib")
#else
#pragma comment(lib, "../x64/Debug/HotReload.lib")
#endif
#endif

//
// Very intrusive and inefficient but allows for adding new fields
// to all allocated heap objects at runtime.
//
#define HR_ALLOC_ADDITIONAL_SPACE_TO_HEAP_OBJECTS 1
#define HR_ALLOC_ADDITIONAL_SPACE_SIZE 128

extern "C"
HOTRELOAD_API
HMODULE HR_LoadLibraryA(LPCSTR lpLibFileName);

extern "C"
HOTRELOAD_API
HMODULE HR_GetModuleHandle(LPCSTR lpLibFileName);
