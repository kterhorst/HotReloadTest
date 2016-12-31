//
// HotReload
//

#pragma once

#define HR_CLEAR_HOTRELOAD_DIRECTORY_ON_START 1

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

extern "C"
HOTRELOAD_API
HMODULE HR_LoadLibraryA(LPCSTR lpLibFileName);

extern "C"
HOTRELOAD_API
HMODULE HR_GetModuleHandle(LPCSTR lpLibFileName);
