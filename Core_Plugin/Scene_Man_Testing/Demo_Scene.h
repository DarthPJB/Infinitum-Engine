#ifndef DEMOKITTEN
#define DEMOKITTEN

#include "Plugin_Static.h"
#define PluginNameDef "Demo_Scene_Manager"

class SceneMan : public SceneServer
{
public:
	SceneMan(PluginManager* Han);
	~SceneMan();
    virtual bool PreInit();                   //does minimal check to ensure this is functional on system.
	virtual bool init();                      //Activate this interface
	virtual bool RegisterServers(RenderServer* RenderI, PhysicsServer* PhysI);
	virtual void RunFrame();
	virtual unsigned int AddObject(std::auto_ptr<World_Object>);
	virtual unsigned int AddPhysObject(std::auto_ptr<World_Phys_Object>);
	virtual unsigned int AddRenderObject(std::auto_ptr<World_Model_Object>);
	virtual void RemoveObject(unsigned int);
	virtual void RemovePhysObject(unsigned int);
	virtual void RemoveRenderObject(unsigned int);
	virtual World_Object* GetObject(unsigned int);
	virtual World_Phys_Object* GetPhysObject(unsigned int);
	virtual World_Model_Object* GetRenderObject(unsigned int);
protected:
	void CalcNonPhysObjects();
	void CalcPhysObjects();
	void CalcRenderObjects();
	RenderServer* RenderServ;
    PhysicsServer* PhysServ;
	unsigned int Objects_CurHigh;
	unsigned int PhysObjects_CurHigh;
	unsigned int RenderObjects_CurHigh;

	std::map<unsigned int,World_Object*> Objects;
	std::map<unsigned int,World_Phys_Object*> PhysObjects;
	std::map<unsigned int,World_Model_Object*> RenderObjects;

	std::map<unsigned int, unsigned int> RenderToPhys;
};
#endif