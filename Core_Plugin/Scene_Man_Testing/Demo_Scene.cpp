#include "Demo_Scene.h"

PluginData PLF_Init()
{
    PluginData RetVal;
    RetVal.Name = PluginNameDef;
    RetVal.Type = Render;
    RetVal.Version.Major_Version = PLF_MAJOR_VERSION;
    RetVal.Version.Minor_Version = PLF_MINOR_VERSION;
    RetVal.Version.Func_Version = PLF_FUNC_VERSION;
    return RetVal;
}
bool PLF_Register(std::auto_ptr<Version_Info> Version_Data, PluginManager * handler)
{
    //simple version check.
    if(Version_Data->Major_Version < PLF_MAJOR_VERSION) return false;
	new SceneMan(handler);
    return true;
}
SceneMan::SceneMan(PluginManager* Han) : SceneServer(Han, PluginNameDef)
{
	Han->RegisterScene(PluginNameDef, std::auto_ptr<SceneServer>(this));
	Objects_CurHigh = 0;
	PhysObjects_CurHigh = 0;
	RenderObjects_CurHigh = 0;
}
SceneMan::~SceneMan()
{
	for(std::map<unsigned int, World_Model_Object*>::iterator IT = RenderObjects.begin(); IT != RenderObjects.end(); IT++)
	{
		RemoveRenderObject(IT->first + 1);
	}
	for(std::map<unsigned int, World_Phys_Object*>::iterator IT = PhysObjects.begin(); IT != PhysObjects.end(); IT++)
	{
		RemovePhysObject(IT->first + 1);
	}
	for(std::map<unsigned int, World_Object*>::iterator IT = Objects.begin(); IT != Objects.end(); IT++)
	{
		RemoveObject(IT->first + 1);
	}
}
bool SceneMan::PreInit()                   //does minimal check to ensure this is functional on system.
{
	ERRSYS = Handler->GrabError();
	return true; //This scene manager is pure c++, it's allways going to work.
}
bool SceneMan::init()                      //Activate this interface
{
	if((PhysServ != NULL) && (RenderServ != NULL)) return true; //This scene manager is pure c++, it's allways going to work.
	else return false; //Can't run without the physics and render servers loaded.
}
bool SceneMan::RegisterServers(RenderServer* RenderI, PhysicsServer* PhysI)
{
	if(PhysI!=NULL) PhysServ = PhysI;
	else return false;
	if(RenderI!=NULL) RenderServ = RenderI;
	else return false;
	return true;
}
unsigned int SceneMan::AddObject(std::auto_ptr<World_Object> Object)
{
	if(Object.get() == NULL)return 0;
    std::pair<std::map<unsigned int, World_Object*>::iterator, bool> RetVal;
	RetVal = Objects.insert(std::make_pair(Objects_CurHigh, Object.get()));
    if(RetVal.second) 
	{
		Objects_CurHigh++;
		Object.release();
	}
    else 
	{
		Object.reset();
		return 0;
	}
    unsigned int Number = RetVal.first->first;
    return Number+1;
}
unsigned int SceneMan::AddPhysObject(std::auto_ptr<World_Phys_Object> Object)
{
	if(Object.get() == NULL)return 0;
    std::pair<std::map<unsigned int, World_Phys_Object*>::iterator, bool> RetVal;
	RetVal = PhysObjects.insert(std::make_pair(PhysObjects_CurHigh, Object.get()));
    if(RetVal.second) 
	{
		PhysObjects_CurHigh++;
		Object.release();
	}
    else 
	{
		Object.reset();
		return 0;
	}
    unsigned int Number = RetVal.first->first;
    return Number+1;
}
unsigned int SceneMan::AddRenderObject(std::auto_ptr<World_Model_Object> Object)
{
	if(Object.get() == NULL)return 0;
    std::pair<std::map<unsigned int, World_Model_Object*>::iterator, bool> RetVal;
	RetVal = RenderObjects.insert(std::make_pair(RenderObjects_CurHigh, Object.get()));
    if(RetVal.second) 
	{
		RenderObjects_CurHigh++;
	}
    else 
	{
		Object.reset();
		return 0;
	}
    unsigned int Number = RetVal.first->first;
	//render objects are phys objects, so we duplicate this in the phys objects.
	RenderToPhys[Number] = AddPhysObject(std::auto_ptr<World_Phys_Object> (Object.release()));
	return Number+1;
}
void SceneMan::RemoveObject(unsigned int Value)
{
	Value--;
    std::map<unsigned int, World_Object*>::iterator Loc= Objects.find(Value);
    if(Loc != Objects.end())
    {
		if(Loc->second != NULL)delete Loc->second;
        Objects.erase(Loc);
    }
}
void SceneMan::RemovePhysObject(unsigned int Value)
{
	Value--;
    std::map<unsigned int, World_Phys_Object*>::iterator Loc= PhysObjects.find(Value);
    if(Loc != PhysObjects.end())
    {
        if(Loc->second != NULL)delete Loc->second;
        PhysObjects.erase(Loc);
    }
}
void SceneMan::RemoveRenderObject(unsigned int Value)
{
	Value--;
    std::map<unsigned int, World_Model_Object*>::iterator Loc= RenderObjects.find(Value);
    if(Loc != RenderObjects.end())
    {
        if(Loc->second != NULL)delete Loc->second;
        RenderObjects.erase(Loc);
    }
	//also find and remove the phys object.
	std::map<unsigned int, World_Phys_Object*>::iterator LocPhys= PhysObjects.find(RenderToPhys[Value]);
    if(LocPhys != PhysObjects.end())
    {
        PhysObjects.erase(LocPhys);
    }
}
World_Object* SceneMan::GetObject(unsigned int Value)
{
	Value--;
    std::map<unsigned int, World_Object*>::iterator Loc= Objects.find(Value);
    if(Loc != Objects.end()) return  Loc->second;
    else return NULL;
}
World_Phys_Object* SceneMan::GetPhysObject(unsigned int Value)
{
	Value--;
    std::map<unsigned int, World_Phys_Object*>::iterator Loc= PhysObjects.find(Value);
    if(Loc != PhysObjects.end()) return  Loc->second;
    else return NULL;
}
World_Model_Object* SceneMan::GetRenderObject(unsigned int Value)
{
	Value--;
    std::map<unsigned int, World_Model_Object*>::iterator Loc= RenderObjects.find(Value);
    if(Loc != RenderObjects.end()) return  Loc->second;
    else return NULL;
}

void SceneMan::RunFrame()
{
	if(RenderServ!=NULL && PhysServ != NULL)
	{
		CalcNonPhysObjects();
		CalcPhysObjects();
		RenderServ->FrameStart();
			CalcRenderObjects();
		RenderServ->FrameEnd();
	}
}

void SceneMan::CalcNonPhysObjects()
{
	//These don't do anything, so.. yeah..
}
void SceneMan::CalcPhysObjects()
{
	PhysServ->CalcSet(&PhysObjects);
}
void SceneMan::CalcRenderObjects()
{
	for(std::map<unsigned int, World_Model_Object*>::iterator IT = RenderObjects.begin(); IT != RenderObjects.end(); IT++)
	{
		RenderServ->RenderModel(IT->second->GetModel(), IT->second->GetModelViewMatrix());
	}
}