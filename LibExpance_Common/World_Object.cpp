#include "World_Object.h"


//						World object Defintions
World_Object::World_Object()
{
	Parent = NULL;
}
World_Object::World_Object(World_Object* ptr)
{
	Parent = ptr->GetParent();
	Translation = *ptr->GetTranslation();
	Rotation = *ptr->GetRotation();
	Scale = *ptr->GetScale();
}
World_Object* World_Object::GetParent()
{
	return Parent;
}
std::vector<World_Object*> World_Object::GetChildren()
{
	return Children;
}
glm::mat4* World_Object::GetTranslation()
{
	return &Translation;
}
glm::mat4* World_Object::GetRotation()
{
	return &Rotation;
}
glm::mat4* World_Object::GetScale()
{
	return &Scale;
}
glm::mat4 World_Object::GetFullMatrix()
{
	//translation is before rotation and scaling
	return Translation * Rotation * Scale;
}
glm::mat4 World_Object::GetModelViewMatrix()
{
	if(Parent != NULL) return Parent->GetModelViewMatrix() * GetFullMatrix();
	else return GetFullMatrix();
}


//						Physical object Defintions
World_Phys_Object::World_Phys_Object()
{
	PhysModel = NULL;
	Scalar_Velocity = glm::vec3(1,1,1);
}
World_Phys_Object::World_Phys_Object(World_Phys_Object* ptr) : World_Object(ptr)
{
	PhysModel = ptr->GetPhysModel();
	Velocity = ptr->GetVel();
	Rotational_Velocity = ptr->GetRotVel();
	Scalar_Velocity = ptr->GetScalar();
}
World_Phys_Object::World_Phys_Object(World_Object* ptr) : World_Object(ptr)
{
	PhysModel = NULL;
}
void World_Phys_Object::AddVel(glm::vec3 Vel)
{
	Velocity += Vel;
	return;
}
void World_Phys_Object::SetVel(glm::vec3 Vel)
{
	Velocity = Vel;
	return;
}
glm::vec3 World_Phys_Object::GetVel()
{
	return Velocity;
}

void World_Phys_Object::AddRotVel(glm::vec3 Rot)
{
	Rotational_Velocity += Rot;
	return;
}
void World_Phys_Object::SetRotVel(glm::vec3 Rot)	
{
	Rotational_Velocity = Rot;
	return;
}
glm::vec3 World_Phys_Object::GetRotVel()
{
	return Rotational_Velocity;
}

void World_Phys_Object::AddScalar(glm::vec3 Scalar)
{
	Scalar_Velocity += Scalar;
	return;
}
void World_Phys_Object::SetScalar(glm::vec3 Scalar)
{
	Scalar_Velocity = Scalar;
	return;
}

void World_Phys_Object::AddScale(glm::vec3 New)
{
	//glm::scale(Scale, New);
	Scale = glm::scale(Scale, New);
	//Scale *= New;
}
void World_Phys_Object::SetScale(glm::vec3 Set)
{
	Scale = glm::scale(glm::mat4(), Set);
}

glm::vec3 World_Phys_Object::GetScalar()
{
	return Scalar_Velocity;
}
Base_Model * World_Phys_Object::GetPhysModel()
{
	return PhysModel;
}
void World_Phys_Object::SetPhysModel(Base_Model* Model)
{
	PhysModel = Model;
	return;
}


//						Model object Defintions
World_Model_Object::World_Model_Object()
{
	Model=NULL;
}
World_Model_Object::World_Model_Object(World_Model_Object* ptr) : World_Phys_Object(ptr)
{
	Model = ptr->GetModel();
}
World_Model_Object::World_Model_Object(World_Phys_Object* ptr) : World_Phys_Object(ptr)
{
	Model = NULL;
}
World_Model_Object::World_Model_Object(World_Object* ptr) : World_Phys_Object(ptr)
{
	Model = NULL;
}
Base_Model * World_Model_Object::GetModel()
{
	return Model;
}
void World_Model_Object::SetModel(Base_Model* DModel)
{
	Model = DModel;
	return;
}