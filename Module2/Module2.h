// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MODULE2_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MODULE2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MODULE2_EXPORTS
#define MODULE2_API __declspec(dllexport)
#else
#define MODULE2_API __declspec(dllimport)
#endif

// This class is exported from the Module2.dll
class MODULE2_API CModule2 {
public:
	CModule2(void);
	// TODO: add your methods here.
};

extern MODULE2_API int nModule2;

extern "C" MODULE2_API int fnModule2(void);
