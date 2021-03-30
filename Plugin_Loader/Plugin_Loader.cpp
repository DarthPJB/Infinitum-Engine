/*
*       The Plugin Loader source file, this defines the core functional class of the
*		Plugin loader, it is designed to be cross-platform capable and support ease of use
*		for plugin developers.
*
*              Created Monday 05/06/2011
* 
*              (C) John Bargman, DarthPJB@gmail.com
*/



#include "Plugin_Loader.h"
#include <map>
#include "boost/filesystem.hpp"
PluginManager_0 MasterHandler;

extern "C"
{
	DLL_PUBLIC bool LoadPlugins(std::string FolderPath)
	{
		try 
		{
			//file-system commands
			std::string File_extension;
			if (!boost::filesystem::is_directory(FolderPath)) return false;
			for(boost::filesystem::directory_iterator Folder(FolderPath); Folder != boost::filesystem::directory_iterator(); Folder++)
			{
				if(boost::filesystem::is_regular_file(*Folder))
				{
					if(Folder->path().has_extension())
					{
						//get file extension
						File_extension = Folder->path().extension().string();
						std::transform(File_extension.begin(), File_extension.end(), File_extension.begin(), ::tolower);
#if _WIN32 || __CYGWIN__
						if(strcmp( File_extension.c_str(), ".dll") ==0)
#else
						if(strcmp( File_extension.c_str(), ".dylib")==0 || strcmp( File_extension.c_str(), "so")==0)
#endif
						{
							//it's a dylib, so try and load it
							LoadPlugin(Folder->path().string());
						}
					}
				}
			}
			return true;
		}
		catch (const boost::filesystem::filesystem_error &Report)
		{
			return false;
		}
		catch (...)
		{
			return false;
		}

	}
    DLL_PUBLIC PluginManager* GetPLF()
    {
        return &MasterHandler;
    }
    DLL_PUBLIC std::string LoadPlugin(std::string Path)
    {
		//prepare failure responce string
        std::string Fail = "Failed";
		//handle for dynamic library
        LibHandle Library = NULL;
		//load library at given path
        Library = PLF_LoadLibrary(Path);
		//attempt to capture standard query function address from library
        Query_PLF_Plugin QueryFunc_P = (Query_PLF_Plugin)PLF_FindFunc(Library,"Query_PLF_Plugin");
        if(QueryFunc_P == NULL)
        {
            //reject plugin
            return Fail;
        }
		//capture plugin data structure
        std::auto_ptr<PluginData> New_Plugin_Data(QueryFunc_P());
        std::auto_ptr<Version_Info> CurVersion = MasterHandler.ReqVersion();
		//local version check (rejecting more-recent plugins) (may be removed in later versions)
        if(New_Plugin_Data->Version > *CurVersion)
        {
            //reject plugin
            return Fail;
        }
		//attempt to capture standard activation function
        Activate_PLF_Plugin Activation_P = (Activate_PLF_Plugin)PLF_FindFunc(Library,"Activate_PLF_Plugin");
        if(Activation_P == NULL)
        {
            //reject plugin
            return Fail;
        }
		//prepare local plugin data structure
        PluginData PlInfo;
        PlInfo.Handle = Library;
        PlInfo.Type = New_Plugin_Data->Type;
        PlInfo.Name = New_Plugin_Data->Name;
        PlInfo.Version = New_Plugin_Data->Version;
        MasterHandler.PluginRef(PlInfo);
        
        //CurVersion CANNOT BE USED AFTER THIS LINE!        
		//Attempt plugin activation (plugin should self register)
        if(!Activation_P(CurVersion, &MasterHandler))
        {
            //reject plugin
            MasterHandler.Remove(New_Plugin_Data->Name);
            return Fail;
        }
        //everything worked (how is this possible?)
        return PlInfo.Name;
    }
}


PluginManager_0::PluginManager_0()
{
    Error_Server = NULL;
}
PluginManager_0::~PluginManager_0()
{
    if(Error_Server != NULL) delete Error_Server;
    for(std::map<std::string,InputServer*>::iterator I=Input_Servers.begin(); I != Input_Servers.end(); I++)
    {
        if((I->second) != NULL)
        {
            delete (I->second);
            (I->second) = NULL;
        }

    }
    Input_Servers.clear();
    for(std::map<std::string,AudioServer*>::iterator I=Audio_Servers.begin(); I != Audio_Servers.end(); I++)
    {
        if((I->second) != NULL)
        {
            delete (I->second);
            (I->second) = NULL;
        }
    }
    Audio_Servers.clear();
    for(std::map<std::string,RenderServer*>::iterator I=Render_Servers.begin(); I != Render_Servers.end(); I++)
    {
        if((I->second) != NULL)
        {
            delete (I->second);
            (I->second) = NULL;
        }
    }
    Render_Servers.clear();
    for(std::map<std::string,SceneServer*>::iterator I=Scene_Servers.begin(); I != Scene_Servers.end(); I++)
    {
        if((I->second) != NULL)
        {
            delete (I->second);
            (I->second) = NULL;
        }
    }
    Scene_Servers.clear();
    for(std::map<std::string,PhysicsServer*>::iterator I=Physics_Servers.begin(); I != Physics_Servers.end(); I++)
    {
        if((I->second) != NULL)
        {
            delete (I->second);
            (I->second) = NULL;
        }
    }
    Physics_Servers.clear();
    for(std::map<std::string,FileClass*>::iterator I=File_Classes.begin(); I != File_Classes.end(); I++)
    {
        if((I->second) != NULL)
        {
            delete (I->second);
            (I->second) = NULL;
        }
    }
    File_Classes.clear();
    for(std::map<std::string,HookClass*>::iterator I=Hook_Classes.begin(); I != Hook_Classes.end(); I++)
    {
        if((I->second) != NULL)
        {
            delete (I->second);
            (I->second) = NULL;
        }
    }
    Hook_Classes.clear();
}

bool PluginManager_0::IsLoaded(std::string Name)
{
    std::map<std::string, PluginData>::iterator InternalList = Registered_Plugins.find(Name);
    if(InternalList != Registered_Plugins.end())    
    {
        return true;
    }
    else
    {
        return false;
    }
};
void PluginManager_0::PluginRef(PluginData PlugInfo)
{
    if(PlugInfo.Name == "unknown") 
    {
        //log error?
        return;
    }
    else
    {
        Registered_Plugins[PlugInfo.Name] = PlugInfo;
        return;
    }
};
std::auto_ptr<Version_Info> PluginManager_0::ReqVersion()
{
    Version_Info * CurVersion = new Version_Info;
    CurVersion->Func_Version = Func_Version;
    CurVersion->Major_Version = Major_Version;
    CurVersion->Minor_Version = Minor_Version;
    return std::auto_ptr<Version_Info> (CurVersion);
};

std::vector<std::string> PluginManager_0::InputServer_List()
{
	std::vector<std::string> RetVal;
	for(std::map<std::string, InputServer*>::iterator Item = Input_Servers.begin(); Item != Input_Servers.end(); Item++)
	{
		RetVal.push_back(Item->first);
	}
	return RetVal;
};

std::vector<std::string> PluginManager_0::FileClass_List()
{
    std::vector<std::string> RetVal;
	for(std::map<std::string, FileClass*>::iterator Item = File_Classes.begin(); Item != File_Classes.end(); Item++)
	{
		RetVal.push_back(Item->first);
	}
	return RetVal;
};
    
std::vector<std::string> PluginManager_0::RenderServer_List()
{
	std::vector<std::string> RetVal;
	for(std::map<std::string,RenderServer*>::iterator Item = Render_Servers.begin(); Item != Render_Servers.end(); Item++)
	{
		RetVal.push_back(Item->first);
	}
	return RetVal;
};


std::vector<std::string> PluginManager_0::AudioServer_List()
{
    std::vector<std::string> RetVal;
	for(std::map<std::string, AudioServer*>::iterator Item = Audio_Servers.begin(); Item != Audio_Servers.end(); Item++)
	{
		RetVal.push_back(Item->first);
	}
	return RetVal;
};


std::vector<std::string> PluginManager_0::SceneServer_List()
{
    std::vector<std::string> RetVal;
	for(std::map<std::string, SceneServer*>::iterator Item = Scene_Servers.begin(); Item != Scene_Servers.end(); Item++)
	{
		RetVal.push_back(Item->first);
	}
	return RetVal;
};

std::vector<std::string> PluginManager_0::HookClass_List()
{
    std::vector<std::string> RetVal;
	for(std::map<std::string, HookClass*>::iterator Item = Hook_Classes.begin(); Item != Hook_Classes.end(); Item++)
	{
		RetVal.push_back(Item->first);
	}
	return RetVal;
};

std::vector<std::string> PluginManager_0::PhysicsServer_List()
{
    std::vector<std::string> RetVal;
	for(std::map<std::string, PhysicsServer*>::iterator Item = Physics_Servers.begin(); Item != Physics_Servers.end(); Item++)
	{
		RetVal.push_back(Item->first);
	}
	return RetVal;
};

void PluginManager_0::Remove(std::string Name) //removes unloaded Plugin-classes
{
    if(IsLoaded(Name))
    {
        std::map<std::string, PluginData>::iterator InternalList = Registered_Plugins.find(Name);
        if(InternalList != Registered_Plugins.end())
        {
            
            //remove plugin references
            PurgeModules(Name);
            
            if(InternalList->second.Modules.size() > 0)
            {
                for(std::vector<std::string>::iterator ModuleName = InternalList->second.Modules.begin(); ModuleName < InternalList->second.Modules.end(); ModuleName++)
                {

                }
            }
            //free library
            PLF_FreeLibrary(InternalList->second.Handle);
            
            //remove reference
            Registered_Plugins.erase(InternalList);
        }
    }
}
void PluginManager_0::PurgeModules(std::string Name)
{
    std::map<std::string, HookClass*>::iterator Hooks = Hook_Classes.find(Name);
    std::map<std::string, FileClass*>::iterator Files = File_Classes.find(Name);
    std::map<std::string, PhysicsServer*>::iterator Physics = Physics_Servers.find(Name);
    std::map<std::string, AudioServer*>::iterator Audio = Audio_Servers.find(Name);
    std::map<std::string, SceneServer*>::iterator Scene = Scene_Servers.find(Name);
    std::map<std::string, RenderServer*>::iterator Render = Render_Servers.find(Name);
    std::map<std::string, InputServer*>::iterator Input = Input_Servers.find(Name);

    if(Hooks != Hook_Classes.end()) 
    {
        if(Hooks->second != NULL) delete Hooks->second;
        Hook_Classes.erase(Hooks);
    }
    if(Files != File_Classes.end()) 
    {
        if(Files->second != NULL) delete Files->second;
        File_Classes.erase(Files);
    }
    if(Physics != Physics_Servers.end()) 
    {
        if(Physics->second != NULL) delete Physics->second;
        Physics_Servers.erase(Physics);
    }
    if(Audio != Audio_Servers.end()) 
    {
        if(Audio->second != NULL)delete Audio->second;
        Audio_Servers.erase(Audio);
    }
    if(Scene != Scene_Servers.end()) 
    {
        if(Scene->second != NULL) delete Scene->second;
        Scene_Servers.erase(Scene);
    }
    if(Render != Render_Servers.end()) 
    {
        if(Render->second != NULL) delete Render->second;
        Render_Servers.erase(Render);
    }
    if(Input != Input_Servers.end())
    {
        if(Input->second != NULL) delete Input->second;
        Input_Servers.erase(Input);
    }
}
void PluginManager_0::RegisterInput(std::string Name,std::auto_ptr<InputServer> Factory_P)
{
    InputServer* Pointer;
    Pointer = Factory_P.release();
    if(Pointer != NULL)
    {
        //Check for a valid name
        if(Pointer->ReqName() == "unknown") 
        {
            delete Pointer;
            return;
        }
        //Check for a valid PreInit signature
        if(!Pointer->PreInit())
        {
            delete Pointer;
            return;
        }
        Input_Servers[Name] = Pointer;
    }
    return;
}
void PluginManager_0::RegisterFile(std::string Name,std::auto_ptr<FileClass> Factory_P)
{
    FileClass* Pointer;
    Pointer = Factory_P.release();
    if(Pointer != NULL)
    {
        //Check for a valid name
        if(Pointer->ReqName() == "unknown") 
        {
            delete Pointer;
            return;
        }
        //Check for a valid PreInit signature
        bool Preint= Pointer->PreInit();
        if(!Preint)
        {
            delete Pointer;
            return;
        }
        File_Classes[Name] = Pointer;
    }
}
void PluginManager_0::RegisterRender(std::string Name,std::auto_ptr<RenderServer> Factory_P)
{
    RenderServer* Pointer;
    Pointer = Factory_P.release();
    if(Pointer != NULL)
    {
        //Check for a valid name
        if(Pointer->ReqName() == "unknown") 
        {
            delete Pointer;
            return;
        }
        //Check for a valid PreInit signature
        if(!Pointer->PreInit())
        {
            delete Pointer;
            return;
        }
        Render_Servers[Name] = Pointer;
    }
}
void PluginManager_0::RegisterAudio(std::string Name,std::auto_ptr<AudioServer> Factory_P)
{
    AudioServer* Pointer;
    Pointer = Factory_P.release();
    if(Pointer != NULL)
    {
        //Check for a valid name
        if(Pointer->ReqName() == "unknown") 
        {
            delete Pointer;
            return;
        }
        //Check for a valid PreInit signature
        if(!Pointer->PreInit())
        {
            delete Pointer;
            return;
        }
        Audio_Servers[Name] = Pointer;
    }
}
void PluginManager_0::RegisterScene(std::string Name,std::auto_ptr<SceneServer> Factory_P)
{
    SceneServer* Pointer;
    Pointer = Factory_P.release();
    if(Pointer != NULL)
    {
        //Check for a valid name
        if(Pointer->ReqName() == "unknown") 
        {
            delete Pointer;
            return;
        }
        //Check for a valid PreInit signature
        if(!Pointer->PreInit())
        {
            delete Pointer;
            return;
        }
        Scene_Servers[Name] = Pointer;
    }
}
void PluginManager_0::RegisterHook(std::string Name,std::auto_ptr<HookClass> Factory_P)
{
    HookClass* Pointer;
    Pointer = Factory_P.release();
    if(Pointer != NULL)
    {
        //Check for a valid name
        if(Pointer->ReqName() == "unknown") 
        {
            delete Pointer;
            return;
        }
        //Check for a valid PreInit signature
        if(!Pointer->PreInit())
        {
            delete Pointer;
            return;
        }
        Hook_Classes[Name] = Pointer;
    }
}
void PluginManager_0::RegisterError(std::string Name,std::auto_ptr<ErrorServer> Factory_P)
{
    ErrorServer * Pointer;
    Pointer = Factory_P.release();
    if(Pointer != NULL)
    {
        //Check for a valid name
        if(Pointer->ReqName() == "unknown") 
        {
            delete Pointer;
            return;
        }
        //Check for a valid PreInit signature
        if(!Pointer->PreInit())
        {
            delete Pointer;
            return;
        }
        //remove existing error server if it exists
        if(Error_Server != NULL)
        {
            delete Error_Server;
            Error_Server = NULL;
        }
        Error_Server = Pointer;
    }
}
void PluginManager_0::RegisterPhysics(std::string Name,std::auto_ptr<PhysicsServer> Factory_P)
{
    PhysicsServer* Pointer;
    Pointer = Factory_P.release();
    if(Pointer != NULL)
    {
        //Check for a valid name
        if(Pointer->ReqName() == "unknown") 
        {
            delete Pointer;
            return;
        }
        //Check for a valid PreInit signature
        if(!Pointer->PreInit())
        {
            delete Pointer;
            return;
        }
        Physics_Servers[Name] = Pointer;
    }
}

std::map<std::string, InputServer*> PluginManager_0::InputServer_Req()
{
    std::map<std::string, InputServer*> ReturnVal = Input_Servers;
    return ReturnVal;
}
std::map<std::string, AudioServer*> PluginManager_0::AudioServer_Req()
{
    std::map<std::string, AudioServer*> ReturnVal = Audio_Servers;
    return ReturnVal;
}
std::map<std::string, SceneServer*> PluginManager_0::SceneServer_Req()
{
    std::map<std::string, SceneServer*> ReturnVal = Scene_Servers;
    return ReturnVal;
}
std::map<std::string, PhysicsServer*> PluginManager_0::PhysicsServer_Req()
{
    std::map<std::string, PhysicsServer*> ReturnVal  = Physics_Servers;
    return ReturnVal;
}
std::map<std::string, RenderServer*> PluginManager_0::RenderServer_Req()
{
    std::map<std::string, RenderServer*> ReturnVal  = Render_Servers;
    return ReturnVal;
}
std::map<std::string, HookClass*> PluginManager_0::HookClass_Req()
{
    std::map<std::string, HookClass*> ReturnVal = Hook_Classes;
    return ReturnVal;
}
std::map<std::string, FileClass*> PluginManager_0::FileClass_Req()
{
    std::map<std::string, FileClass*> ReturnVal = File_Classes;
    return ReturnVal;
}

InputServer* PluginManager_0::GrabInput(std::string Name)
{
    std::map<std::string, InputServer*>::iterator Index = Input_Servers.find(Name);
    if(Index != Input_Servers.end())
    {
        return Index->second;
    }
    else
    {
        return NULL;
    }
}
SceneServer* PluginManager_0::GrabScene(std::string Name)
{
    std::map<std::string, SceneServer*>::iterator Index = Scene_Servers.find(Name);
    if(Index != Scene_Servers.end())
    {
        return Index->second;
    }
    else
    {
        return NULL;
    }
}
AudioServer* PluginManager_0::GrabAudio(std::string Name)
{
    std::map<std::string, AudioServer*>::iterator Index = Audio_Servers.find(Name);
    if(Index != Audio_Servers.end())
    {
        return Index->second;
    }
    else
    {
        return NULL;
    }
}
PhysicsServer* PluginManager_0::GrabPhysics(std::string Name)
{
    std::map<std::string, PhysicsServer*>::iterator Index = Physics_Servers.find(Name);
    if(Index != Physics_Servers.end())
    {
        return Index->second;
    }
    else
    {
        return NULL;
    }
}
RenderServer* PluginManager_0::GrabRender(std::string Name)
{
    std::map<std::string, RenderServer*>::iterator Index = Render_Servers.find(Name);
    if(Index != Render_Servers.end())
    {
        return Index->second;
    }
    else
    {
        return NULL;
    }
}
HookClass* PluginManager_0::GrabHook(std::string Name)
{
    std::map<std::string, HookClass*>::iterator Index = Hook_Classes.find(Name);
    if(Index != Hook_Classes.end())
    {
        return Index->second;
    }
    else
    {
        return NULL;
    }
}
FileClass* PluginManager_0::GrabFile(std::string Name)
{
    std::map<std::string, FileClass*>::iterator Index = File_Classes.find(Name);
    if(Index != File_Classes.end())
    {
        return Index->second;
    }
    else
    {
        return NULL;
    }
}
ErrorServer* PluginManager_0::GrabError()
{
    return Error_Server;
}

InputServer* PluginManager_0::GrabInput()
{
    std::map<std::string, InputServer*>::iterator Index = Input_Servers.begin();
    if(Index != Input_Servers.end())
    {
        return Index->second;
    }
    else
    {
        return NULL;
    }
}
SceneServer* PluginManager_0::GrabScene()
{
    std::map<std::string, SceneServer*>::iterator Index = Scene_Servers.begin();
    if(Index != Scene_Servers.end())
    {
        return Index->second;
    }
    else
    {
        return NULL;
    }
}
AudioServer* PluginManager_0::GrabAudio()
{
    std::map<std::string, AudioServer*>::iterator Index = Audio_Servers.begin();
    if(Index != Audio_Servers.end())
    {
        return Index->second;
    }
    else
    {
        return NULL;
    }
}
PhysicsServer* PluginManager_0::GrabPhysics()
{
    std::map<std::string, PhysicsServer*>::iterator Index = Physics_Servers.begin();
    if(Index != Physics_Servers.end())
    {
        return Index->second;
    }
    else
    {
        return NULL;
    }
}
RenderServer* PluginManager_0::GrabRender()
{
    std::map<std::string, RenderServer*>::iterator Index = Render_Servers.begin();
    if(Index != Render_Servers.end())
    {
        return Index->second;
    }
    else
    {
        return NULL;
    }
}
HookClass* PluginManager_0::GrabHook()
{
    std::map<std::string, HookClass*>::iterator Index = Hook_Classes.begin();
    if(Index != Hook_Classes.end())
    {
        return Index->second;
    }
    else
    {
        return NULL;
    }
}
FileClass* PluginManager_0::GrabFile()
{
    std::map<std::string, FileClass*>::iterator Index = File_Classes.begin();
    if(Index != File_Classes.end())
    {
        return Index->second;
    }
    else
    {
        return NULL;
    }
}