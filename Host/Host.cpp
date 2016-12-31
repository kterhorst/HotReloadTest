// Host.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../HotReload/HotReload.h"
#include "../HRSupport/HRSupport.h"
#include "../Module1/Module1.h"

typedef int(*TfnModule2)();

// Export as _TestMain to make it easier to find in debug tools.
extern "C" void __declspec(dllexport) _TestMain()
{
	//
	// Load Module1
	//
	HMODULE module1 = HR_LoadLibraryA("Module1.dll");
	if (!module1)
		throw;
	auto fn1 = reinterpret_cast<TNewCalculator>(
		GetProcAddress(module1, "NewCalculator"));
	if (!fn1)
		throw;

	//
	// Load Module2
	//
	HMODULE module2 = HR_LoadLibraryA("Module2.dll");
	if (!module2)
		throw;
	auto fn2 = reinterpret_cast<TfnModule2>(
		GetProcAddress(module2, "fnModule2"));
	if (!fn2)
		throw;

	ICalculator* calc = fn1();
	
	while (true)
	{
		int x = calc->DoSomeStuff(1, 2);
		printf("Module1: calc->DoSomeStuff(...): %d\n", x);
		x = fn2();
		printf("Module2: fnModule2(): %d\n", x);
		system("pause");
	
		// Magic!
		module1 = HR_LoadLibraryA("Module1.dll");
		module2 = HR_LoadLibraryA("Module2.dll");
		if (!module1 || !module2)
			throw;
	}
}

int main()
{
	_TestMain();
    return 0;
}

