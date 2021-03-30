#include "Phys_Serv.h"

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
	new Phys_Serv(handler);
    return true;
}
Phys_Serv::Phys_Serv(PluginManager * Han) : PhysicsServer(Han, PluginNameDef)
{
	Handler->RegisterPhysics(PluginNameDef, std::auto_ptr<PhysicsServer>(this));
}
Phys_Serv::~Phys_Serv()
{
}
bool Phys_Serv::PreInit()
{
	ERRSYS = Handler->GrabError();
	return true;
}
bool Phys_Serv::init()
{
	if(SceneServ != NULL)return true;
	else return false;
}
bool Phys_Serv::RegisterScene(SceneServer* Scene)
{
	SceneServ = Scene;
	if(SceneServ != NULL)return true;
	else return false;
}
void Phys_Serv::CalcSet(std::map<unsigned int, World_Phys_Object*>* Object_Set)
{
	for(std::map<unsigned int, World_Phys_Object*>::iterator IT = Object_Set->begin(); IT != Object_Set->end(); IT++)
	{
		CalcObject(IT->second);
	}
	return;
}
void Phys_Serv::CalcObject(World_Phys_Object* Object)
{
	//Really should have a 'position' 'rotation' and 'scale' variable that can be added and subtracted to, rather than a matrix off the bat.
	glm::mat4* translation = Object->GetTranslation();
	glm::mat4* rotation = Object->GetRotation();
	glm::mat4* scale = Object->GetScale();
	*translation = glm::translate(*translation, Object->GetVel());
	*rotation = glm::rotate(*rotation, Object->GetRotVel().x,glm::vec3(1,0,0));
	*rotation = glm::rotate(*rotation, Object->GetRotVel().y,glm::vec3(0,1,0));
	*rotation = glm::rotate(*rotation, Object->GetRotVel().z,glm::vec3(0,0,1));
	*scale = glm::scale(*scale,Object->GetScalar());
}