#ifndef Phys_Serv_Testing
#define Phys_Serv_Testing

#define PluginNameDef "Demo_Physics_Server"
#include "Plugin_Static.h"

class Phys_Serv : public PhysicsServer
{
public:
	Phys_Serv(PluginManager * Han);
	~Phys_Serv();
	virtual bool PreInit();
    virtual bool init();
	virtual bool RegisterScene(SceneServer* Scene);
	virtual void CalcSet(std::map<unsigned int, World_Phys_Object*>*);
private:
	void CalcObject(World_Phys_Object*);
};
#endif