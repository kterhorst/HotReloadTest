//
// HotReload
//

#pragma once

#define HR_ENABLED 1

//
// Put HR_ALLOW_FIELD_EXTENSION at the end of your class/struct to add
// some additional space at the end of objects. This allows for adding
// new fields without exceeding the allocated memory of type instances.
// This works for objects on the stack and heap, unless the size of all 
// new fields exceeds the specified placeholder size.
//

#if HR_ENABLED
#define HR_DEFAULT_PLACEHOLDER_SIZE 128
#define _HR_ALLOW_FIELD_EXTENSION(N) char _placeholder_[N]
#else
#define HR_DEFAULT_PLACEHOLDER_SIZE 0
#define _HR_ALLOW_FIELD_EXTENSION(N)
#endif
#define HR_ALLOW_FIELD_EXTENSION _HR_ALLOW_FIELD_EXTENSION(HR_DEFAULT_PLACEHOLDER_SIZE)

//
// Very intrusive and inefficient but allows for adding new fields
// to all allocated _final_ _heap_ objects at runtime. 
// Not really a good idea ;) since this is applied to all allocated 
// objects, doesn't work for ones allocated on the stack and fails
// if the class/struct is inside an inheritance chain.
//
#define HR_ALLOC_ADDITIONAL_SPACE_TO_HEAP_OBJECTS 0
#define HR_ALLOC_ADDITIONAL_SPACE_SIZE HR_DEFAULT_PLACEHOLDER_SIZE

#ifdef NDEBUG
#pragma comment(lib, "../x64/Release/HRSupport.lib")
#else
#pragma comment(lib, "../x64/Debug/HRSupport.lib")
#endif
