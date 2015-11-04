// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the NATIVEPHANTOMMANAGER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// NATIVEPHANTOMMANAGER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef NATIVEPHANTOMMANAGER_EXPORTS
#define NATIVEPHANTOMMANAGER_API __declspec(dllexport)
#else
#define NATIVEPHANTOMMANAGER_API __declspec(dllimport)
#endif

// This class is exported from the NativePhantomManager.dll
class NATIVEPHANTOMMANAGER_API CNativePhantomManager {
public:
	CNativePhantomManager(void);
	// TODO: add your methods here.
};

extern NATIVEPHANTOMMANAGER_API int nNativePhantomManager;

NATIVEPHANTOMMANAGER_API int fnNativePhantomManager(void);
