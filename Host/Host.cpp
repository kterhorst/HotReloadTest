// Host.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../HotReload/HotReload.h"
#include "../HRSupport/HRSupport.h"
#include "../Module1/Module1.h"

// Export as _TestMain to make it easier to find in debug tools.
extern "C" void __declspec(dllexport) _TestMain()
{
	HMODULE module = HR_LoadLibraryA("Module1.dll");
	if (!module)
		throw;

	auto fn = reinterpret_cast<TNewCalculator>(
		GetProcAddress(module, "NewCalculator"));

	ICalculator* calc = fn();
	
	while (true)
	{
		int x = calc->DoSomeStuff(1, 2);
		printf("Result: %d\n", x);
		system("pause");
	
		// Magic!
		module = HR_LoadLibraryA("Module1.dll");
		if (!module)
			throw;
	}
}

int main()
{
	_TestMain();
    return 0;
}

