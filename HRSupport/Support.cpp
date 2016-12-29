//
// Support
//

#include "stdafx.h"
#include "HRSupport.h"

#if HR_ALLOC_ADDITIONAL_SPACE_TO_HEAP_OBJECTS

#pragma warning(disable : 4290)

void operator delete (void* ptr) throw ()
{
	free(ptr);
}

void operator delete[](void* ptr) throw ()
{
	free(ptr);
}

void operator delete (void* ptr, const std::nothrow_t &) throw ()
{
	free(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t &) throw ()
{
	free(ptr);
}

void* operator new (std::size_t size) throw (std::bad_alloc)
{
	void* ptr = malloc(size + HR_ALLOC_ADDITIONAL_SPACE_SIZE);
	if (!ptr)
	{
		throw std::bad_alloc();
	}
	return ptr;
}

void* operator new[](std::size_t size) throw (std::bad_alloc)
{
	void* ptr = malloc(size + HR_ALLOC_ADDITIONAL_SPACE_SIZE);
	if (!ptr)
	{
		throw std::bad_alloc();
	}
	return ptr;
}

void* operator new (std::size_t size, const std::nothrow_t &) throw ()
{
	return malloc(size + HR_ALLOC_ADDITIONAL_SPACE_SIZE);
}

void* operator new[](std::size_t size, const std::nothrow_t &) throw ()
{
	return malloc(size + HR_ALLOC_ADDITIONAL_SPACE_SIZE);
}

#endif
