#include "Core_Files.h"
#include "Plugin_Static.h"

PluginData PLF_Init()
{
    PluginData RetVal;
    RetVal.Name = "Core_File_Types";
    RetVal.Type = File;
    RetVal.Version.Major_Version = PLF_MAJOR_VERSION;
    RetVal.Version.Minor_Version = PLF_MINOR_VERSION;
    RetVal.Version.Func_Version = PLF_FUNC_VERSION;
    return RetVal;
}
bool PLF_Register(std::auto_ptr<Version_Info> Version_Data, PluginManager * Handler)
{
    //simple version check.
    if(Version_Data->Major_Version < PLF_MAJOR_VERSION) return false;
    new OBJ_Model(Handler);
    return true;
}