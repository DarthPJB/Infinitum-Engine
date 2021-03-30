#include "Model_Format.h"

Base_Mesh::Base_Mesh()
{
    //assign default starting options
    Type = Wire;
    RenderMode = Tri;
    SpecialUsed = false;
    SpecialIndex1 = 0;
    SpecialIndex2 = 0;
    SpecialIndex3 = 0;
    
    IndexList = NULL;
    CleanIndex = false;
    VertexList = NULL;
    CleanVertex = false;
    ColourList = NULL;
    CleanColour = false;
    NormalList = NULL;
    CleanNormal = false;
    InterLeaved = NULL;
    CleanInterLeaved = false;
    EdgeList = NULL;
    CleanEdge = false;
    UVList = NULL;
    CleanUV = false;
}
Base_Mesh::~Base_Mesh()
{
    if(CleanColour && ColourList!=NULL)delete ColourList;
    if(CleanVertex && VertexList!=NULL)delete VertexList;
    if(CleanIndex && IndexList!=NULL)delete IndexList;
    if(CleanEdge && EdgeList!=NULL)delete EdgeList;
    if(CleanNormal && NormalList!=NULL)delete NormalList;
    if(CleanUV && UVList!=NULL)delete UVList;
    if(CleanInterLeaved && InterLeaved!=NULL)delete InterLeaved;
}
void Base_Mesh::Optimise()
{
    //interleave arrays or something here
}
List_Data<unsigned int> * Base_Mesh::IndexUsed()
{
    if(IndexList == NULL) return NULL;
    if(!IndexList->CurrentlyUsed()) return NULL;
    else return IndexList;
}
List_Data<float> * Base_Mesh::VertexUsed()
{
    if(VertexList == NULL) return NULL;
    if(!VertexList->CurrentlyUsed()) return NULL;
    else return VertexList;
}
List_Data<float> * Base_Mesh::ColourUsed()
{
    if(ColourList == NULL) return NULL;
    if(!ColourList->CurrentlyUsed()) return NULL;
    else return ColourList;
}
List_Data<float> * Base_Mesh::NormalUsed()
{
    if(NormalList == NULL) return NULL;
    if(!NormalList->CurrentlyUsed()) return NULL;
    else return NormalList;
}
List_Data<float> * Base_Mesh::UVUsed()
{
    if(UVList == NULL) return NULL;
    if(!UVList->CurrentlyUsed()) return NULL;
    else return UVList;
}
List_Data<bool> * Base_Mesh::EdgeUsed()
{
    if(EdgeList == NULL) return NULL;
    if(!EdgeList->CurrentlyUsed()) return NULL;
    else return EdgeList;
}
List_Data_leaved<float> * Base_Mesh::InterleavedUsed()
{
    if(InterLeaved == NULL) return NULL;
    if(!InterLeaved->CurrentlyUsed()) return NULL;
    else return InterLeaved;
}
void Base_Mesh::AssignIndexList(List_Data<unsigned int> * List, bool Cleanup)
{
    IndexList=List;
    CleanIndex = Cleanup;
}
void Base_Mesh::AssignEdgeList(List_Data<bool> *List, bool Cleanup)
{
    EdgeList=List;
    CleanEdge = Cleanup;
}
void Base_Mesh::AssignVertexList(List_Data<float> *List, bool Cleanup)
{
    VertexList=List;
    CleanVertex = Cleanup;
}
void Base_Mesh::AssignNormalList(List_Data<float> *List, bool Cleanup)
{
    NormalList=List;
    CleanNormal = Cleanup;
}
void Base_Mesh::AssignColourList(List_Data<float> *List, bool Cleanup)
{
    ColourList=List;
    CleanColour = Cleanup;
}
void Base_Mesh::AssignInterleavedList(List_Data_leaved<float> *List, bool Cleanup)
{
    InterLeaved=List;
    CleanInterLeaved = Cleanup;
}
void Base_Mesh::SetIndexClean(bool Value)
{
    CleanIndex = Value;
}
void Base_Mesh::SetNormalClean(bool Value)
{
    CleanNormal = Value;
}
void Base_Mesh::SetColourClean(bool Value)
{
    CleanColour = Value;
}
void Base_Mesh::SetVertexClean(bool Value)
{
    CleanVertex = Value;
}
void Base_Mesh::SetEdgeClean(bool Value)
{
    CleanEdge = Value;
}
void Base_Mesh::SetUVClean(bool Value)
{
    CleanUV = Value;
}
void Base_Mesh::SetInterleavedClean(bool Value)
{
    CleanInterLeaved = Value;
}
bool Base_Model::AddMesh(std::auto_ptr<Base_Mesh> ptr)
{
    if(ptr.get() != NULL)
    {
        Meshes.push_back(ptr.release());
        return true;
    }
    return false;
}
Base_Model::Base_Model()
{
    Meshes.clear();
}
Base_Model::~Base_Model()
{
    for(std::vector<Base_Mesh*>::iterator Cur_Mesh=Meshes.begin(); Cur_Mesh < Meshes.end(); Cur_Mesh++)
    {
        if((*Cur_Mesh)!=NULL) delete (*Cur_Mesh);
    }
}
void Base_Model::Optimise()
{
    for(std::vector<Base_Mesh*>::iterator Cur_Mesh=Meshes.begin(); Cur_Mesh < Meshes.end(); Cur_Mesh++)
    {
        if((*Cur_Mesh)!=NULL) (*Cur_Mesh)->Optimise();
    }
}