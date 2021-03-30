#include "Plugin_Header.h"
typedef std::string (*LoadPluginF)(std::string);
typedef PluginManager* (*GetPLFF)();
typedef bool (*LoadPluginsF)(std::string);
    
class PluginFrameWork
{
public:
    PluginFrameWork()
    {
        LoadPluginI = NULL;
        GetPLFI = NULL;
        
        //load PLF_library
        PLFDLL = PLF_LoadLibrary(LibraryPath);
        //assign function handles
        if(PLFDLL!=NULL)
        {
			LoadPluginsI = (LoadPluginsF)PLF_FindFunc(PLFDLL, "LoadPlugins");
            LoadPluginI = (LoadPluginF)PLF_FindFunc(PLFDLL, "LoadPlugin");
            GetPLFI = (GetPLFF)PLF_FindFunc(PLFDLL, "GetPLF");
        }
        return;
    }
    ~PluginFrameWork()
    {
        PLF_FreeLibrary(PLFDLL);
        return;
    }
	//function to load all plugins in a given folder
	bool LoadPlugins(std::string FolderPath)
	{
		if(LoadPluginsI != NULL) return LoadPluginsI(FolderPath);
		else return false;
	}
    //function to load plugin given file-path
    std::string LoadPlugin(std::string path)
    {
        if(LoadPluginI!=NULL)return LoadPluginI(path);
        else return "PLF_NOT_LOADED\n";
    }
    PluginManager* GetPLF()
    {
        if(GetPLFI!=NULL) return GetPLFI();
        else return NULL;
    }
private:
    LibHandle PLFDLL;
    LoadPluginF LoadPluginI;
    GetPLFF GetPLFI;
	LoadPluginsF LoadPluginsI;
};