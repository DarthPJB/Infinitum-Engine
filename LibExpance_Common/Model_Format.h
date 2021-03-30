/* 
 * File:   Model_Format.h
 * Author: John Bargman (c) DarthPJB@gmail.com
 *
 * Created on 03 September 2011, 20:24
 */

#ifndef MODEL_FORMAT_H
#define	MODEL_FORMAT_H
#include <memory>
#include <stddef.h>             //needed for NULL definition in Linux
#include <vector>

template <class DataType> class List_Data
{
protected:
    bool Clean;         //should this list delete the assigned data-pointer?
    bool Assigned;      //is this list used?
    DataType * ListData;        //data pointer
    unsigned int BlockSize;     //number of values per vertex-point in the data
    unsigned int NumElements;   //Number of vertex points in the data
    int ListStride;             //stride (in bytes) between each element
    
public:
    List_Data()
    {
        NumElements = 0;
        ListData = NULL;
        ListStride=0;
        Assigned=false;
        Clean = false;
        BlockSize = 3;
    }
    ~List_Data()
    {
        if(Clean==true) // locally remove lists
        {
            delete [] ListData;
        }
    }
    bool AssignListData(DataType* List, unsigned int Number_Elements, bool Cleanup)
    {
        ListData = List;
        NumElements = Number_Elements;
        Assigned = true;
        Clean = Cleanup;
		return true;
    }
    bool AssignListData(DataType* List, unsigned int Number_Elements, int Stride, bool Cleanup)
    {
        ListStride = Stride;
        return AssignListData(List, Number_Elements, Cleanup);
    }
    DataType * GetListData()
    {
        return ListData;
    }

    unsigned int GetBlockSize()
    {
        return BlockSize;
    }
    void SetBlockSize(unsigned int Size)
    {
        BlockSize = Size;
    }
    unsigned int GetNumElements()
    {
        return NumElements;
    }
    unsigned int GetStride()
    {
        return ListStride;
    }
    bool CurrentlyUsed()
    {
        return Assigned;
    }
};

template <class DataType> class List_Data_leaved 
{
public:
    //header definitions for template-based functions.
    bool AssignListData(DataType* List, unsigned int Number_Elements, int Stride, bool Cleanup)
    {
        ListStride = Stride;
        ListData = List;
        NumElements = Number_Elements;
        Assigned = true;
        Clean = Cleanup;
		return true;
    }
    DataType * GetListData()
    {
        return ListData;
    }
    DataType * GetListData(char type)
    {
        switch (type)
        {
            case 'N':
            case 'n':
                return &ListData[NormalOffset];
                break;
            case 'c':
            case 'C':
                return &ListData[ColourOffset];
                break;
            case 'U':
            case 'u':
                return &ListData[UVOffset];
                break;
            case 'V':
            case 'v':
                return &ListData[VertexOffset];
                break;
			default:
				return NULL;
				break;
        }
    }
    unsigned int GetNumElements()
    {
        return NumElements;
    }
    unsigned int GetStride()
    {
        return ListStride;
    }
    bool CurrentlyUsed()
    {
        return Assigned;
    }
    
    //values for interleaved array
    List_Data_leaved()
    {            
        NumElements = 0;
        ListData = NULL;
        ListStride=0;
        Assigned=false;
        Clean = false;
        
        Normal=false;               //Is this used?
        NormalBlockSize=0;          //Size of Data-blocks
        NormalOffset=0;             //offset from 0
        Vertex=false;            
        VertexBlockSize=0;
        VertexOffset=0;
        Colour=false;            
        ColourBlockSize=0;
        ColourOffset=0;
        UV=false;            
        UVBlockSize=0;
        UVOffset=0;
    }
    ~List_Data_leaved()
    {
        if(Clean==true) // locally remove lists
        {
            delete [] ListData;
        }
    }
    void SetColour(int Offset, int size)
    {
        ColourBlockSize=size;
        ColourOffset=Offset;
        Colour = true;
    }
    void SetNormal(int Offset, int size)
    {
        NormalBlockSize=size;
        NormalOffset=Offset;
        Normal = true;
    }
    void SetVertex(int Offset, int size)
    {
        VertexBlockSize=size;
        VertexOffset=Offset;
        Vertex = true;
    }
    void SetUV(int Offset, int size)
    {
        UVBlockSize=size;
        UVOffset=Offset;
        UV = true;
    }
    unsigned int GetBlockOffset(char type)
    {
        switch (type)
        {
            case 'N':
            case 'n':
                return NormalOffset;
                break;
            case 'c':
            case 'C':
                return ColourOffset;
                break;
            case 'U':
            case 'u':
                return UVOffset;
                break;
            case 'V':
            case 'v':
                return VertexOffset;
                break;
            default:
                return 0;
                break;
        }
    }
    unsigned int GetBlockSize(char type)
    {
        switch (type)
        {
            case 'N':
            case 'n':
                return NormalBlockSize;
                break;
            case 'c':
            case 'C':
                return ColourBlockSize;
                break;
            case 'U':
            case 'u':
                return UVBlockSize;
                break;
            case 'V':
            case 'v':
                return VertexBlockSize;
                break;
            default:
                return 0;
                break;
        }
    }
    unsigned int GetBlockSize()
    {
        return NormalBlockSize + ColourBlockSize + UVBlockSize +VertexBlockSize;
    }
    bool CurrentlyUsed(char type)
    {
        switch (type)
        {
            case 'N':
            case 'n':
                return Normal;
                break;
            case 'c':
            case 'C':
                return Colour;
                break;
            case 'U':
            case 'u':
                return UV;
                break;
            case 'V':
            case 'v':
                return Vertex;
                break;
			default:
				return NULL;
				break;
        }     
    }
protected:
    bool Clean;         //should this list delete the assigned data-pointer?
    bool Assigned;      //is this list used?
    DataType * ListData;        //data pointer
    unsigned int NumElements;   //Number of vertex points in the data
    int ListStride;             //stride (in bytes) between each element
    
    //these functions are overridden as protected to prevent their use (and generate compiler error)
    void SetBlockSize(unsigned int Size);
    int ColourBlockSize;
    int NormalBlockSize;
    int UVBlockSize;
    int VertexBlockSize;

    int ColourOffset;
    int NormalOffset;
    int UVOffset;
    int VertexOffset;

    bool Colour;
    bool Normal;
    bool UV;
    bool Vertex;
};


typedef List_Data<float> Base_Vertex_List;
typedef List_Data<float> Base_Colour_List;
typedef List_Data<float> Base_Normal_List;
typedef List_Data<float> Base_UV_List;
typedef List_Data<bool> Base_Edge_List;
typedef List_Data_leaved<float> Base_Interleaved_List;
typedef List_Data<unsigned int> Base_Index_List;


class Base_Mesh
{
public:
    enum MeshType {Wire=0, Solid=1,Smooth=2, TexSmooth=3, TexSolid=4, NON_RENDER=5, Deformable=10, Other_Mesh=100};
    enum RenderType {Tri=0, TriStrip=1, Quad=2, QuadStrip=3, Polygon=4, Line=5, LineStrip=6, Other_Render=100};
    Base_Mesh();
    ~Base_Mesh();
    //graphics lists (not all need to be used but must be avaliable to be used)
    Base_Index_List * IndexUsed();
    Base_Vertex_List * VertexUsed();
    Base_Colour_List * ColourUsed();
    Base_Normal_List * NormalUsed();
    Base_Vertex_List * UVUsed();
    Base_Edge_List * EdgeUsed();
    Base_Interleaved_List * InterleavedUsed();
    void AssignIndexList(Base_Index_List *, bool Cleanup);
    void AssignEdgeList(Base_Edge_List *, bool Cleanup);
    void AssignVertexList(Base_Vertex_List *, bool Cleanup);
    void AssignNormalList(Base_Normal_List *, bool Cleanup);
    void AssignColourList(Base_Colour_List *, bool Cleanup);
    void AssignInterleavedList(Base_Interleaved_List *, bool Cleanup);
    
    void SetIndexClean(bool Value);
    void SetNormalClean(bool Value);
    void SetColourClean(bool Value);
    void SetVertexClean(bool Value);
    void SetEdgeClean(bool Value);
    void SetUVClean(bool Value);
    void SetInterleavedClean(bool Value);
    
    void Optimise();
    //These HWspecials are in unsigned long int format so they can be used to:
        //hold keyvalues provided by the renderer
        //hold pointers to data provided by the renderer
        //fire lasers from the renderer
    //Only the renderer should ever use these, and how it does is up to it's
    //implimentation
    unsigned long int SpecialIndex1;    //Used for buffer objects and other HW options
    unsigned long int SpecialIndex2;    //Used for buffer objects and other HW options
    unsigned long int SpecialIndex3;    //Used for buffer objects and other HW options
    MeshType Type;
    RenderType RenderMode;
    bool SpecialUsed;
private:
    Base_Index_List * IndexList;
    bool CleanIndex;
    Base_Vertex_List * VertexList;
    bool CleanVertex;
    Base_Colour_List * ColourList;
    bool CleanColour;
    Base_Normal_List * NormalList;
    bool CleanNormal;
    Base_UV_List * UVList;
    bool CleanUV;
    Base_Edge_List * EdgeList;
    bool CleanEdge;
    Base_Interleaved_List * InterLeaved;
    bool CleanInterLeaved;
};

class Base_Model
{
public:
    bool AddMesh(std::auto_ptr<Base_Mesh>);
    void Optimise();
    Base_Model();
    ~Base_Model();
    std::vector<Base_Mesh*> Meshes;
};
#endif	/* MODEL_FORMAT_H */

