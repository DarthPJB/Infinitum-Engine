#include "Common_Library.h"


#if defined _WIN32 || defined __CYGWIN__      /*Compiling under windows*/
    #define	PLATFORM_MANAGED
    #define LibraryPath "./Plugin_Loader.dll"
    #include <windows.h>
    #define LibHandle HINSTANCE 
    typedef unsigned long int FuncProc;
    #include <string>

    LibHandle PLF_LoadLibrary(std::string LibPath)
    {
        LibHandle RetVal = LoadLibrary(LibPath.c_str());
        if (!RetVal) {
            RetVal = NULL;
        }
        return RetVal;
    }

    void PLF_FreeLibrary(LibHandle Handle)
    {
        if(Handle != NULL)
        {
            FreeLibrary(Handle);
        }
    }

    FuncProc PLF_FindFunc(LibHandle Handle, std::string FuncName)
    {
        // reset errors

            FuncProc RetVal =(FuncProc) GetProcAddress(Handle, FuncName.c_str());
        if (!RetVal) 
        {
            RetVal = NULL;
        }
        return RetVal;
    }
    #ifdef BUILDING_DLL
    #ifdef __GNUC__
        #define DLL_PUBLIC __attribute__((dllexport))
    #elif  _MSC_VER >= 1400 /*Visual studio 2005+*/
        #define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
    #else
    #ifdef __GNUC__
        #define DLL_PUBLIC __attribute__((dllimport))
    #elif  _MSC_VER >= 1400 /*Visual studio 2005+*/
        #define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
    #endif
    #define DLL_LOCAL
#endif                                        /*compiling under linux*/
#if __GNUC__ >= 4 &&  defined __linux__
    #define DLL_PUBLIC __attribute__ ((visibility("default")))
    #define DLL_LOCAL  __attribute__ ((visibility("hidden")))
    #ifndef PLATFORM_MANAGED
    #define	PLATFORM_MANAGED
    #define LibraryPath "./libplugin_loader.so"
    #define LibHandle void *
    typedef unsigned long int FuncProc;
    #include <string>
    #include <dlfcn.h>

    LibHandle PLF_LoadLibrary(std::string LibPath)
    {
        dlerror();
        LibHandle RetVal = dlopen(LibPath.c_str(), RTLD_LAZY);
        const char* dlsym_error = dlerror();
        if (!RetVal) {
            RetVal = NULL;
        }
        return RetVal;

    }

    void PLF_FreeLibrary(LibHandle Handle)
    {
        if(Handle != NULL)
        {
            dlclose(Handle);
        }
    }

    FuncProc PLF_FindFunc(LibHandle Handle, std::string FuncName)
    {
        // reset errors
        dlerror();

        FuncProc RetVal = (FuncProc) dlsym(Handle, FuncName.c_str());
        const char* dlsym_error = dlerror();
        if (dlsym_error) 
        {
            RetVal =(FuncProc) NULL;
        }
        return RetVal;
    }
    #endif
#endif
