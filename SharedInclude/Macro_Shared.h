/*
 *      Macro for library export-import functions
 *              Note: if using this with G++ you must use the
 *              -fvisibility=hidden command line option.
 *
 *              Created Saturday 04/06/2011 
 * 
 *      Edit Friday 02/09/2011
 *              Included Dynamic library loading wrappers
 *              Added Define for Library file name
 * 
 *              (C) John Bargman
 *
 */

#ifndef MACRO_SHARED_H
#define	MACRO_SHARED_H

    //Macro for export / import functions

    #if defined _WIN32 || defined __CYGWIN__      /*Compiling under windows*/
        #define	PLATFORM_MANAGED
        #define LibraryPath "./Plugin_Loader.dll"
        #include <windows.h>
        #define LibHandle HINSTANCE 
        typedef unsigned long int FuncProc;
        #include <string>

        LibHandle PLF_LoadLibrary(std::string LibPath);
        void PLF_FreeLibrary(LibHandle Handle);
        FuncProc PLF_FindFunc(LibHandle Handle, std::string FuncName);
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

        LibHandle PLF_LoadLibrary(std::string LibPath);
        void PLF_FreeLibrary(LibHandle Handle);
        FuncProc PLF_FindFunc(LibHandle Handle, std::string FuncName);
        #endif
    #endif

#endif	/* MACRO_SHARED_H */
