// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MODULE1_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MODULE1_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MODULE1_EXPORTS
#define MODULE1_API __declspec(dllexport)
#else
#define MODULE1_API __declspec(dllimport)
#endif

#include <functional>

struct ICalculator
{
	virtual int DoSomeStuff(int x, int y) = 0;
	virtual std::function<int()> GetSomeLambda() = 0;
};

typedef ICalculator* (*TNewCalculator)(void);
