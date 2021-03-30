/*
 *      This is the Plugin Header, it contains shared definitions used by both the plugin loader 
 *      and the plugin static library
 * 
 *              Created Friday 03/06/2011
 * 
 *      (C) John Bargman, DarthPJB@gmail.com
 * 
  */


#ifndef PLF_H
#define PLF_H
#define BUILDING_DLL
#include <memory>
#include <map>
#include "Macro_Shared.h"

#ifndef PLF_VERSION
    /* Plugin Framework Version, update this on changes.   -- Major, minor, function additions
    * Major changes will explicitly reject any other major version plugin.
    * Minor changes will allow any plugin within that major version
    * function additions are for reference only */
    #define PLF_MAJOR_VERSION 0
    #define PLF_MINOR_VERSION 5
    #define PLF_FUNC_VERSION 5
#endif

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Produce shared header for this library
#include "../LibExpance_Common/Common_Library.h"
/* These are the base classes for use by the plugin libraries.
 * these classes are used to provide base pointers to the defined classes
 * that are then passed between the plugin and the loader to provide functionallity.*/

//structure and enumeration used to pass plugin information back to process
enum PlugType {File=1, Render=2, Audio=3, Scene=4, Hook=5, Physics=6, Input=7, Error=8, Multi=0};
//class predefinitions (so classes may refer to others)
class PluginManager;
class InputServer;
class FileClass;
class HookClass;
class AudioServer;
class RenderServer;
class SceneServer;
class PhysicsServer;
class ErrorServer;
class PLF_Plugin;
class Version_Info;
struct PluginData;

//*************************plugin master defintion******************************
class PluginManager //abstract plugin manager class
{
public:
    PluginManager()
    {
        Major_Version = PLF_MAJOR_VERSION;
        Minor_Version = PLF_MINOR_VERSION;
        Func_Version = PLF_FUNC_VERSION;
    }
    ~PluginManager()
    {
        Major_Version = 0;
        Minor_Version =0;
        Func_Version =0;
    }
public:
    virtual bool IsLoaded(std::string)=0;
    virtual void Remove(std::string)=0;
    virtual void PluginRef(PluginData)=0;
    virtual std::auto_ptr<Version_Info> ReqVersion()=0;

    virtual void RegisterInput(std::string, std::auto_ptr<InputServer>)=0;
    virtual std::map<std::string, InputServer*> InputServer_Req()=0;
    virtual std::vector<std::string> InputServer_List()=0;
    virtual InputServer* GrabInput(std::string Name)=0;
	virtual InputServer* GrabInput()=0;

    virtual void RegisterFile(std::string, std::auto_ptr<FileClass>)=0;
    virtual std::map<std::string, FileClass*> FileClass_Req()=0;
    virtual std::vector<std::string> FileClass_List()=0;
    virtual FileClass* GrabFile(std::string Name)=0;
	virtual FileClass* GrabFile()=0;
    
    virtual void RegisterRender(std::string, std::auto_ptr<RenderServer>)=0;
    virtual std::map<std::string, RenderServer*> RenderServer_Req()=0;
    virtual std::vector<std::string> RenderServer_List()=0;
    virtual RenderServer* GrabRender(std::string Name)=0;
	virtual RenderServer* GrabRender()=0;


    virtual void RegisterAudio(std::string, std::auto_ptr<AudioServer>)=0;
    virtual std::vector<std::string> AudioServer_List()=0;
    virtual std::map<std::string, AudioServer*> AudioServer_Req()=0;
    virtual AudioServer* GrabAudio(std::string Name)=0;
	virtual AudioServer* GrabAudio()=0;


    virtual void RegisterScene(std::string, std::auto_ptr<SceneServer>)=0;
    virtual std::map<std::string, SceneServer*> SceneServer_Req()=0;
    virtual std::vector<std::string> SceneServer_List()=0;
    virtual SceneServer* GrabScene(std::string Name)=0;
	virtual SceneServer* GrabScene()=0;

    virtual void RegisterHook(std::string, std::auto_ptr<HookClass>)=0;
    virtual std::vector<std::string> HookClass_List()=0;
    virtual std::map<std::string, HookClass*> HookClass_Req()=0;
    virtual HookClass* GrabHook(std::string Name)=0;
	virtual HookClass* GrabHook()=0;

    virtual void RegisterPhysics(std::string, std::auto_ptr<PhysicsServer>)=0;
    virtual std::vector<std::string> PhysicsServer_List()=0;
    virtual std::map<std::string, PhysicsServer*> PhysicsServer_Req()=0;
    virtual PhysicsServer* GrabPhysics(std::string Name)=0;
	virtual PhysicsServer* GrabPhysics()=0;

    virtual void RegisterError(std::string, std::auto_ptr<ErrorServer>)=0;
    virtual ErrorServer* GrabError()=0; //can only have one error server
protected:
    unsigned int Major_Version;
    unsigned int Minor_Version;
    unsigned int Func_Version;
};


//parent class definition
class PLF_Plugin
{
public:
    PLF_Plugin(PluginManager* Han, std::string PluginName)
    {
        Name = PluginName;
        Handler = Han;
        ERRSYS=NULL;
    }
    std::string ReqName() { return Name; }
virtual bool PreInit()=0;                   //does minimal check to ensure this is functional on system.
virtual bool init()=0;                      //Activate this interface
protected:
    std::string Name;
    PluginManager* Handler;
    ErrorServer * ERRSYS;
};

/*********************** BASE CLASS abstract definitions*********************/
class HookClass : public PLF_Plugin
{
public:
    HookClass(PluginManager* Han, std::string name) : PLF_Plugin (Han, name)
    {
        //null values to be replaced during init
        prehook = "";
        posthook = "";
        Handler->RegisterHook(Name,std::auto_ptr<HookClass>(this));
        return;
    }
    virtual bool PreInit()=0;                   //does minimal check to ensure this is functional on system.
    virtual bool init()=0;                      //Activate this interface
    virtual std::string PreHookName()
    {
        return prehook;
    }
    virtual std::string PostHookName()
    {
        return posthook;
    }
    virtual bool prehookfunc(void *)=0;
    virtual bool posthookfunc(void *)=0;
private:
    std::string prehook;
    std::string posthook;
};

class FileClass : public PLF_Plugin
{
public:
    FileClass(PluginManager* Han, std::string name) : PLF_Plugin (Han, name)
    {
        Extention = "";
        return;
    }
    virtual bool PreInit()=0;
    virtual bool init()=0;
    virtual std::string GetExtention()=0;
    virtual unsigned int Load(std::string)=0;
    virtual std::string Write(std::string, void *)=0;
    virtual std::string Write(std::string, unsigned int)=0;
    virtual void * Select(unsigned int)=0;
    virtual void Free(unsigned int)=0;
private:
    std::string Extention;
};

class InputServer : public PLF_Plugin
{
public:
    InputServer(PluginManager* Han, std::string name) : PLF_Plugin (Han, name)
    {return;};
    enum Device {Pointer0 =0,Pointer1 =1,Pointer2 =2,Pointer3 =3,Pointer4 =4,
                Pointer5 =5,Pointer6 =6,Pointer7 =7,Pointer8 =8,Pointer9 =9,
                Keyboard=10};
    virtual bool PreInit()=0;                   //does minimal check to ensure this is functional on system.
    virtual bool init()=0;                      //Activate this interface
    virtual PointDev* PointerDevice(Device)=0;
    virtual bool* GetKeys()=0;
	virtual void FakePointerInput(Device,glm::vec2)=0;
    virtual void FakeKeyDown(unsigned int Key)=0;
    virtual void FakeKeyUp(unsigned int Key)=0;
private:
    bool Keys[255];
    
};

//these definitions need filling out with appropriate data
class AudioServer : public PLF_Plugin
{
public:
    AudioServer(PluginManager* Han, std::string name) : PLF_Plugin (Han, name)
    {return;};
    virtual bool PreInit()=0;                   //does minimal check to ensure this is functional on system.
    virtual bool init()=0;                      //Activate this interface
};

class RenderServer : public PLF_Plugin
{
public:
    RenderServer(PluginManager* Han, std::string name) : PLF_Plugin (Han, name)
    {return;};
    virtual bool PreInit()=0;                   //does minimal check to ensure this is functional on system.
    virtual bool init()=0;						//Activate this interface (with optional data)
    virtual void CloseRender()=0;
    virtual void RenderMesh(Base_Mesh *)=0;						//render a mesh
	virtual void RenderModel(Base_Model *, glm::mat4)=0;		//render a model
    virtual void FrameEnd()=0;
    virtual void FrameStart()=0;
    virtual void ReScaleView(int w, int h,bool threeD)=0;
    virtual void ReScaleWindow(int w, int h)=0;
    virtual bool ToggleFullScreen(bool)=0;
    virtual void OptimiseMesh(Base_Mesh *)=0;
    virtual void OptimiseModel(Base_Model *)=0;
    virtual void FreeModelData(Base_Model *)=0;
    virtual void FreeMeshData(Base_Mesh *)=0;
};  

class SceneServer : public PLF_Plugin
{
public:
    SceneServer(PluginManager* Han, std::string name) : PLF_Plugin (Han, name)
    {
		RenderServ = NULL;
		PhysServ = NULL;
		return;
	};
    virtual bool PreInit()=0;                   //does minimal check to ensure this is functional on system.
	virtual bool init()=0;                      //Activate this interface
	virtual bool RegisterServers(RenderServer* RenderI, PhysicsServer* PhysI)=0;
	virtual unsigned int AddObject(std::auto_ptr<World_Object>)=0;
	virtual unsigned int AddPhysObject(std::auto_ptr<World_Phys_Object>)=0;
	virtual unsigned int AddRenderObject(std::auto_ptr<World_Model_Object>)=0;
	virtual void RemoveObject(unsigned int)=0;
	virtual void RemovePhysObject(unsigned int)=0;
	virtual void RemoveRenderObject(unsigned int)=0;
	virtual World_Object* GetObject(unsigned int)=0;
	virtual World_Phys_Object* GetPhysObject(unsigned int)=0;
	virtual World_Model_Object* GetRenderObject(unsigned int)=0;
    virtual void RunFrame()=0;
protected:
	RenderServer* RenderServ;
    PhysicsServer* PhysServ;
};


class PhysicsServer : public PLF_Plugin
{
public:
    PhysicsServer(PluginManager* Han, std::string name) : PLF_Plugin (Han, name)
    {
		SceneServ = NULL;
		return;
	};
	virtual bool PreInit()=0;
    virtual bool init()=0;
	virtual bool RegisterScene(SceneServer* Scene)=0;
	virtual void CalcSet(std::map<unsigned int, World_Phys_Object*>*)=0;
protected:
	SceneServer* SceneServ;
private:
};

class ErrorServer : public PLF_Plugin
{
public:
    ErrorServer(PluginManager* Han, std::string name) : PLF_Plugin (Han, name)
    {return;};
    virtual bool init()=0;
    virtual void Write(unsigned short ErrorLevel, std::string String, std::string Dump)=0;
    virtual void Write(unsigned short ErrorLevel, std::string String, std::string Dump, std::string ExtendedData)=0;
    virtual bool ReadLast(char** String, unsigned int* Length)=0;
    virtual void ChangeLogging(bool LoggerStatus)=0;	//Used to Enable or Disable ErrSys.
    virtual bool GetLoggingStatus()=0;				//Used to retreive the current status of the logger (Enabled/Disabled)
    virtual void SetShutdownFunc(void (*ShutDownFunc)(bool))=0;
    virtual void Throw(std::string)=0;
};

//*********************** Plugin Data defintions*************************
class Version_Info
{
public:
    unsigned int Major_Version;
    unsigned int Minor_Version;
    unsigned int Func_Version;

    bool operator>(Version_Info Comp)
    {
        if(Comp.Major_Version < Major_Version)
        {
            return false;
        }
        if(Comp.Major_Version > Major_Version)
        {
            return true;
        }
        if(Comp.Major_Version == Major_Version)
        {
            if(Comp.Minor_Version < Minor_Version)
            {
                return false;
            }
            if(Comp.Minor_Version > Minor_Version)
            {
                return true;
            }
            if(Comp.Minor_Version == Minor_Version)
            {
                if(Comp.Func_Version <= Func_Version)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
        }
		return false;
    }
    bool operator!=(Version_Info Comp)
    {
        if(Comp.Major_Version == Major_Version)
        {
            return false;
        }
        if(Comp.Minor_Version == Minor_Version)
        {
            return false;
        }
        if(Comp.Func_Version == Func_Version)
        {
            return false;
        }
        return true;
    }
    bool operator>=(Version_Info Comp)
    {
        if(Comp.Major_Version < Major_Version)
        {
            return false;
        }
        if(Comp.Major_Version > Major_Version)
        {
            return true;
        }
        if(Comp.Major_Version == Major_Version)
        {
            if(Comp.Minor_Version < Minor_Version)
            {
                return false;
            }
            if(Comp.Minor_Version > Minor_Version)
            {
                return true;
            }
            if(Comp.Minor_Version == Minor_Version)
            {
                if(Comp.Func_Version < Func_Version)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
        }
    }
    bool operator<(Version_Info Comp)
    {
        if(Comp.Major_Version > Major_Version)
        {
            return false;
        }
        if(Comp.Major_Version < Major_Version)
        {
            return true;
        }
        if(Comp.Major_Version == Major_Version)
        {
            if(Comp.Minor_Version > Minor_Version)
            {
                return false;
            }
            if(Comp.Minor_Version < Minor_Version)
            {
                return true;
            }
            if(Comp.Minor_Version == Minor_Version)
            {
                if(Comp.Func_Version >= Func_Version)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
        }
    }
    bool operator==(Version_Info Comp)
    {
        if(Comp.Major_Version != Major_Version)
        {
            return false;
        }
        if(Comp.Minor_Version != Minor_Version)
        {
            return false;
        }
        if(Comp.Func_Version != Func_Version)
        {
            return false;
        }
        return true;
    }
    bool operator<=(Version_Info Comp)
    {
        if(Comp.Major_Version > Major_Version)
        {
            return false;
        }
        if(Comp.Major_Version < Major_Version)
        {
            return true;
        }
        if(Comp.Major_Version == Major_Version)
        {
            if(Comp.Minor_Version > Minor_Version)
            {
                return false;
            }
            if(Comp.Minor_Version < Minor_Version)
            {
                return true;
            }
            if(Comp.Minor_Version == Minor_Version)
            {
                if(Comp.Func_Version > Func_Version)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
        }
    }
};

struct PluginData
{
    LibHandle Handle;
    PlugType Type;
    Version_Info Version;
    std::string Name;
    std::vector<std::string> Modules;
};
#endif