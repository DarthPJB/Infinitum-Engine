/*
*       The Plugin Loader Header file, this defines the core functional class of the
*      Plugin loader, it is designed to be cross-platform capable and really cool.
*
*              Created Saturday 04/06/2011
* 
*              (C) John Bargman, DarthPJB@gmail.com
*/

#ifndef BUILDING_DLL
    #define BUILDING_DLL
#endif


#ifndef PLF
#define PLF
#include "Plugin_Header.h"
#include <string>
#include <vector>
#include <memory>

extern "C"
{
	//function to load plugins from a given folder
	DLL_PUBLIC bool LoadPlugins(std::string);
    //function to load plugin given file-path
    DLL_PUBLIC std::string LoadPlugin(std::string);
    //function to return Master-interface
    DLL_PUBLIC PluginManager* GetPLF();
}

typedef bool (*Activate_PLF_Plugin)(std::auto_ptr<Version_Info>, PluginManager*);
typedef PluginData* (*Query_PLF_Plugin)();

// different versions of the plugin manager may be added here if needed
// however inheriting and overriding the previous version is required.

class PluginManager_0 : public PluginManager
{
public:
    PluginManager_0();
    ~PluginManager_0();
    
    virtual bool IsLoaded(std::string);
    virtual void Remove(std::string);
    virtual void PluginRef(PluginData);
    virtual std::auto_ptr<Version_Info> ReqVersion();

    virtual void RegisterInput(std::string, std::auto_ptr<InputServer>);
    virtual std::map<std::string, InputServer*> InputServer_Req();
    virtual std::vector<std::string> InputServer_List();
    virtual InputServer* GrabInput(std::string Name);
	virtual InputServer* GrabInput();

    virtual void RegisterFile(std::string, std::auto_ptr<FileClass>);
    virtual std::map<std::string, FileClass*> FileClass_Req();
    virtual std::vector<std::string> FileClass_List();
    virtual FileClass* GrabFile(std::string Name);
	virtual FileClass* GrabFile();
    
    virtual void RegisterRender(std::string, std::auto_ptr<RenderServer>);
    virtual std::map<std::string, RenderServer*> RenderServer_Req();
    virtual std::vector<std::string> RenderServer_List();
    virtual RenderServer* GrabRender(std::string Name);
	virtual RenderServer* GrabRender();


    virtual void RegisterAudio(std::string, std::auto_ptr<AudioServer>);
    virtual std::vector<std::string> AudioServer_List();
    virtual std::map<std::string, AudioServer*> AudioServer_Req();
    virtual AudioServer* GrabAudio(std::string Name);
	virtual AudioServer* GrabAudio();


    virtual void RegisterScene(std::string, std::auto_ptr<SceneServer>);
    virtual std::map<std::string, SceneServer*> SceneServer_Req();
    virtual std::vector<std::string> SceneServer_List();
    virtual SceneServer* GrabScene(std::string Name);
	virtual SceneServer* GrabScene();

    virtual void RegisterHook(std::string, std::auto_ptr<HookClass>);
    virtual std::vector<std::string> HookClass_List();
    virtual std::map<std::string, HookClass*> HookClass_Req();
    virtual HookClass* GrabHook(std::string Name);
	virtual HookClass* GrabHook();

    virtual void RegisterPhysics(std::string, std::auto_ptr<PhysicsServer>);
    virtual std::vector<std::string> PhysicsServer_List();
    virtual std::map<std::string, PhysicsServer*> PhysicsServer_Req();
    virtual PhysicsServer* GrabPhysics(std::string Name);
	virtual PhysicsServer* GrabPhysics();

    virtual void RegisterError(std::string, std::auto_ptr<ErrorServer>);
    virtual ErrorServer* GrabError(); //can only have one error server
   
private:
    void PurgeModules(std::string);
    std::map<std::string, PluginData> Registered_Plugins;
    std::map<std::string, InputServer*> Input_Servers;
    std::map<std::string, FileClass*> File_Classes;
    std::map<std::string, HookClass*> Hook_Classes;
    std::map<std::string, AudioServer*> Audio_Servers;
    std::map<std::string, RenderServer*> Render_Servers;
    std::map<std::string, SceneServer*> Scene_Servers;
    std::map<std::string, PhysicsServer*> Physics_Servers;
    ErrorServer * Error_Server;     //THERE CAN ONLY BE ONE!!!!
}extern MasterHandler;
#endif