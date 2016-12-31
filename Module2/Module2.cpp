// Module2.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Module2.h"


// This is an example of an exported variable
MODULE2_API int nModule2=0;

// This is an example of an exported function.
extern "C" MODULE2_API int fnModule2(void)
{
    return 423;
}

// This is the constructor of a class that has been exported.
// see Module2.h for the class definition
CModule2::CModule2()
{
    return;
}
