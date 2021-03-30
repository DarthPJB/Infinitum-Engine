#ifndef WORLD_OBJECT_H
#define WORLD_OBJECT_H

#include "Model_Format.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>

class World_Object
{
public:
	World_Object();
	World_Object(World_Object*);
	glm::mat4* GetTranslation();
	glm::mat4* GetRotation();
	glm::mat4* GetScale();
	glm::mat4 GetFullMatrix();
	glm::mat4 GetModelViewMatrix();
	World_Object* GetParent();
	std::vector<World_Object*> GetChildren();
protected:
	glm::mat4 Translation;
	glm::mat4 Rotation;
	glm::mat4 Scale;
	std::string Name;
	unsigned int ID;
	std::vector<World_Object*> Children;
	World_Object* Parent;

};

class World_Phys_Object : public World_Object
{
public:
	World_Phys_Object();
	World_Phys_Object(World_Phys_Object*);
	World_Phys_Object(World_Object*);
	void AddVel(glm::vec3);
	void SetVel(glm::vec3);
	glm::vec3 GetVel();

	void AddRotVel(glm::vec3);
	void SetRotVel(glm::vec3);
	glm::vec3 GetRotVel();

	void AddScalar(glm::vec3);
	void SetScalar(glm::vec3);

	void AddScale(glm::vec3);
	void SetScale(glm::vec3);
	glm::vec3 GetScalar();
	Base_Model * GetPhysModel();
	void SetPhysModel(Base_Model*);
private:
	Base_Model * PhysModel;
	glm::vec3 Velocity;
	glm::vec3 Rotational_Velocity;
	glm::vec3 Scalar_Velocity;
};

class World_Model_Object : public World_Phys_Object
{
public:
	World_Model_Object();
	World_Model_Object(World_Model_Object*);
	World_Model_Object(World_Phys_Object*);
	World_Model_Object(World_Object*);
	Base_Model * GetModel();
	void SetModel(Base_Model*);
private:
	Base_Model * Model;
};


#endif