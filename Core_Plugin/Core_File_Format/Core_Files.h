/* 
 * File:   OBJModel.h
 * Author: pjb
 *
 * Created on 10 September 2011, 02:05
 */

#ifndef OBJMODEL_H
#define	OBJMODEL_H
#include "Plugin_Header.h"
#include <memory>
#include <map>
#include <vector>
#include <stdio.h>
#include <cstring>

class OBJ_Model : public FileClass
{
public:
    OBJ_Model(PluginManager*);
    ~OBJ_Model();
   
    virtual bool PreInit();
    virtual bool init();
    virtual std::string GetExtention();
    virtual unsigned int Load(std::string);
    virtual std::string Write(std::string, void *);
    virtual std::string Write(std::string, unsigned int);
    virtual void * Select(unsigned int);
    virtual void Free(unsigned int);
private: 
    unsigned int Add(Base_Model*);
    void Clear(unsigned int);
    std::string Write(std::string, Base_Model*);
    unsigned int LoadOBJ(FILE* TheMeshFile,std::string FilePath);
    void readstr(FILE *f,char *string);      // Read In A String
    
    unsigned int AddVectorIndex(glm::ivec3 VecIndex);
    unsigned int AddVectorIndex(int Vert, int Norm, int UV);
    
    std::vector<glm::ivec4>	Vector_Index_Buffer;	//this has to go in the class to deal with loading for OBJ files via the AddVectorIndex function
    
    unsigned int CurHigh;
    
    //PJBMaterial*	MeshMaterial;
    unsigned int Last_QuadIndex;
    unsigned int Last_TriIndex;
    unsigned int Last_EdgeIndex;
    std::map<unsigned int, Base_Model*> Models;
    
    struct Mesh_Component
    {
        unsigned int Low_QuadIndex;
        unsigned int Low_TriIndex;
        unsigned int Low_EdgeIndex;
        unsigned int QuadIndex;
        unsigned int TriIndex;
        unsigned int EdgeIndex;
//            PJBMaterial* Material;
        bool Smoothing;
    };
    struct Mesh_Sub_Component
    {
            enum BaseType {Edges =0,Triangles=1, Quads=2} Type;
            std::vector<unsigned int> IndexList;
    };
    struct Array_One_Line
    {
            unsigned int Index;
            float Vertex[3];
            float Normal[3];
            float UV[2];
    };
};
#endif	/* OBJMODEL_H */

