/* 
 * File:   Plugin_Static.h
 * Author: John Bargman
 *
 * *****************************************************************************
 * ************************** IMPORTANT NOTE ***********************************
 * *    Plugins compiled MUST export the Query and Activate Symbols            *
 * * On linux this is preformed with                                           *
 * *       -wl, -u Query_PLF_Plugin -u Activate_PLF_Plugin                     *
 * 
 * *****************************************************************************
 * Created on 28 June 2011, 07:53 (c) John Bargman
 */

#ifndef PLUGIN_STATIC_H
#define	PLUGIN_STATIC_H
#ifndef BUILDING_DLL
	#define BUILDING_DLL
#endif

#include "Plugin_Header.h"
#include <memory>

//functions that MUST be defined by the user per plugin
PluginData PLF_Init();
bool PLF_Register(std::auto_ptr<Version_Info>, PluginManager *);


extern "C"
{
    DLL_PUBLIC PluginData* Query_PLF_Plugin()
    {
        PluginData * ReturnData = new PluginData;
        *ReturnData = PLF_Init();
        Version_Info Version;
        Version.Func_Version = PLF_FUNC_VERSION;
        Version.Major_Version = PLF_MAJOR_VERSION;
        Version.Minor_Version = PLF_MINOR_VERSION;
        ReturnData->Version = Version;
        return ReturnData;
    }
    DLL_PUBLIC bool Activate_PLF_Plugin(std::auto_ptr<Version_Info> Engine_Version, PluginManager * PLF_MAN)
    {
        //we simply pass this data to the user function, it's then up to the use to commit a failure if the 
        // engine version is incompatible with this plugin.
        return PLF_Register(Engine_Version, PLF_MAN);
    }
}
#endif	/* PLUGIN_STATIC_H */

