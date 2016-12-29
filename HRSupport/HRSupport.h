//
// HotReload
//

#pragma once

//
// Very intrusive and inefficient but allows for adding new fields
// to all allocated _heap_ objects at runtime.
//
#define HR_ALLOC_ADDITIONAL_SPACE_TO_HEAP_OBJECTS 1
#define HR_ALLOC_ADDITIONAL_SPACE_SIZE 128

#ifdef NDEBUG
#pragma comment(lib, "../x64/Release/HRSupport.lib")
#else
#pragma comment(lib, "../x64/Debug/HRSupport.lib")
#endif
