#include "Core_Files.h"
std::string OBJ_Model::Write(std::string, Base_Model*)
{
	return "ERROR";
}
OBJ_Model::OBJ_Model(PluginManager* Han) : FileClass (Han, "OBJ")
{
    Handler->RegisterFile(Name,std::auto_ptr<FileClass>(this));
}
OBJ_Model::~OBJ_Model()
{
    for(std::map<unsigned int,Base_Model*>::iterator Cur_Model=Models.begin(); Cur_Model != Models.end(); Cur_Model++)
    {
        if(Cur_Model->second != NULL)
        {
            delete Cur_Model->second;
        }
    }
}
bool OBJ_Model::PreInit()
{
    Models.clear();    
    return true;
}
bool OBJ_Model::init()
{
    CurHigh = 0;
    ERRSYS = Handler->GrabError();
    return true;
}
std::string OBJ_Model::GetExtention()
{
    return "obj";
}
unsigned int OBJ_Model::Load(std::string FilePath)
{
	FILE* TheMeshFile=NULL;
        TheMeshFile = fopen(FilePath.c_str(), "rb");

	if(TheMeshFile) return LoadOBJ(TheMeshFile, FilePath);
	else 
	{
            if(ERRSYS!=NULL)
		ERRSYS->Write(25, "Failed To Load OBJ Model", FilePath.c_str());
		return 0;
	}
}
std::string OBJ_Model::Write(std::string Path, void * ptr)
{
    return Write(Path, (Base_Model*) ptr);
}
std::string OBJ_Model::Write(std::string Path, unsigned int Model)
{
	Model--;
    return Write(Path, Models[Model]);
}
void * OBJ_Model::Select(unsigned int Model)
{
	Model--;
    std::map<unsigned int, Base_Model*>::iterator Loc= Models.find(Model);
    if(Loc != Models.end())
    {
        return (void*) Loc->second;
    }
    else
    {
        return NULL;
    }
}
void OBJ_Model::Free(unsigned int Model)
{
	Model--;
    std::map<unsigned int, Base_Model*>::iterator Loc= Models.find(Model);
    if(Loc != Models.end())
    {
        delete Loc->second;
        Models.erase(Loc);
    }
}
void OBJ_Model::readstr(FILE *f,char *string)      // Read In A String
{
	 do               // Start A Loop
	 {
		  fgets(string, 255, f);        // Read One Line
		  // See If It Is Worthy Of Processing
	 } while (!feof(f) && ((string[0] == '/') || (string[0] == '\n') || (string[0] == '#')));
	 return;             // Jump Back
}

unsigned int OBJ_Model::AddVectorIndex(glm::ivec3 VecIndex)
{
        bool SearchWorked=false;
        for(unsigned int i=0; i < Vector_Index_Buffer.size(); i++)
        {
                if(Vector_Index_Buffer.at(i).y==VecIndex.x) //vertex index
                {
                        if(Vector_Index_Buffer.at(i).w==VecIndex.y) //UV index
                        {
                                if(Vector_Index_Buffer.at(i).z==VecIndex.z) //Normal Index
                                {
                                        SearchWorked=true;
                                        return Vector_Index_Buffer.at(i).x;
                                }
                        }
                }
        }
        glm::ivec4 NewIndex(Vector_Index_Buffer.size()-1,VecIndex.x,VecIndex.y,VecIndex.z);
        unsigned int TempVal=Vector_Index_Buffer.size();
        Vector_Index_Buffer.push_back(NewIndex); 
        return TempVal;

}
unsigned int OBJ_Model::AddVectorIndex(int Vert, int Norm, int UV)
{
        bool SearchWorked=false;
        for(unsigned int i=0; i < Vector_Index_Buffer.size(); i++)
        {
                if(Vector_Index_Buffer.at(i).y==Vert) //Vertex Index
                {
                        if(Vector_Index_Buffer.at(i).z==UV) //UV Index
                        {
                                if(Vector_Index_Buffer.at(i).w==Norm) //Normal Index
                                {
                                        SearchWorked=true;
                                        return Vector_Index_Buffer.at(i).x;
                                }
                        }
                }
        }
        glm::ivec4 NewIndex(Vector_Index_Buffer.size(),Vert,Norm,UV);
        unsigned int TempVal=Vector_Index_Buffer.size();
        Vector_Index_Buffer.push_back(NewIndex); 
        return TempVal;
}

unsigned int OBJ_Model::LoadOBJ(FILE* TheMeshFile, std::string FilePath)
{
    bool status = true;
    Base_Model * InternalModel = NULL;
    char oneline[255];           // String To Store Data In
    char readString[255];
    bool Smoothing = false;
    //assign vectors for buffering storing data
    std::vector<glm::vec3 >			VertexBuffer;
    std::vector<glm::vec3 >			NormalBuffer;
    std::vector<glm::vec2>			UVBuffer;
    std::vector<Array_One_Line>                 ArrayBuffer;
    std::vector<Mesh_Component>                 MeshRef;
    //assign buffers for Storing Final Data.
    float* ModelArray;
    Mesh_Sub_Component		QuadIndexBuffer;
    Mesh_Sub_Component		TriIndexBuffer;
    Mesh_Sub_Component		EdgeIndexBuffer;
    //set buffers to correct types
    QuadIndexBuffer.Type =	Mesh_Sub_Component::Quads;
    TriIndexBuffer.Type =	Mesh_Sub_Component::Triangles;
    EdgeIndexBuffer.Type =	Mesh_Sub_Component::Edges;

    unsigned int VertCount=0;		//number of verts
    unsigned int UVCount=0;			//number of Uv coords
    unsigned int NormalCount=0;		//number of Normals
    unsigned int FaceCount=0;       //number of faces
    //PJBMaterial* CurMat=NULL;		//current material
    if(TheMeshFile)
    {
// ******************************* SCAN SECTION (find number of values) *******
        readstr(TheMeshFile,oneline);         // Get Single Line Of Data
        while(!feof(TheMeshFile))
        {
                sscanf(oneline, "%255s",&readString);
                if(!strcmp(readString,"mtllib"))
                {
                      /*  char ReadVal[255];
                        for(unsigned int i=strlen("mtllib "), Count=0; i<=strlen(oneline); i++, Count++)
                        {
                                if((oneline[i] != 10) && (oneline[i] != 13))
                                {
                                        ReadVal[Count] = oneline[i];
                                }
                                else
                                {
                                        ReadVal[Count] = '\0';
                                        break;
                                }
                        }
                        char* OpenFile;
                        OpenFile = new char[strlen(ReadVal) + strlen(FilePath)+1];
                        for(unsigned int i=0;i<strlen(FilePath);i++)
                        {
                                if((FilePath[i] != '\n') && (FilePath[i] != 10) && (FilePath[i] != 13))
                                {
                                                OpenFile[i]=FilePath[i];
                                }
                        }
                        for(unsigned int i=strlen(FilePath),Count=0;i<strlen(FilePath)+strlen(ReadVal);i++)
                        {
                                if((ReadVal[Count] != '\n') && (ReadVal[Count] != 10) && (ReadVal[Count] != 13))	
                                {
                                        OpenFile[i]=ReadVal[Count];
                                }
                                else
                                {
                                        OpenFile[i]='\0';
                                }
                                Count++;
                        }
                        OpenFile[strlen(ReadVal) + strlen(FilePath)] = '\0';
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<PJBLoadMTL(OpenFile,TextureMan);
                        delete [] OpenFile;*/
                }
                else if(!strcmp(readString,"v"))
                {
                        float Val1,Val2,Val3;
                        sscanf(oneline, "%*s %f %f %f\n", &Val1,&Val2,&Val3);
                        glm::vec3 Vert_Buffer(Val1,Val2,Val3);
                        VertexBuffer.push_back(Vert_Buffer);
                        VertCount++;
                }
                else if(!strcmp(readString,"vn"))
                {
                        float Val1,Val2,Val3;
                        sscanf(oneline, "%*s %f %f %f\n", &Val1,&Val2,&Val3);
                        glm::vec3 Norm_Buffer(Val1,Val2,Val3);
                        NormalBuffer.push_back(Norm_Buffer);
                        NormalCount++; 
                }
                else if(!strcmp(readString,"vt"))
                {
                        float Val1,Val2;
                        sscanf(oneline, "%*s %f %f\n", &Val1,&Val2);
                        glm::vec2 UV_Buffer(Val1,Val2);
                        UVBuffer.push_back(UV_Buffer);
                        UVCount++;
                }
                readstr(TheMeshFile,oneline);         // Get Single Line Of Data
        }

        //we have now read every UV and vertex avalible, so now we need to start
        //adding meshes,faces,and materials
        rewind(TheMeshFile);		//set read location to the file start
        readstr(TheMeshFile,oneline);         // Get Single Line Of Data
        while(!feof(TheMeshFile))
        {	//start reading faces and groups
// ******************************* LOAD SECTION (Load all values) *************
                sscanf(oneline, "%255s",&readString);
                if(!strcmp(readString,"usemtl"))
                {
                        char ReadVal[255];
                        sscanf(oneline, "%*s %s \n", ReadVal);
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CurMat = TextureMan->CheckMaterial(ReadVal);
                }
                else if(!strcmp(readString,"s off"))
                {
                        Smoothing = false;
                }
                else if(!strcmp(readString,"s"))
                {
                        Smoothing = true;
                }
                if(!strcmp(readString,"f"))
                {
                        unsigned int Norm[4];			//holds normal Index
                        unsigned int Vert[4],UV[4];		//Holds Vert and UV Index
                        if(sscanf(oneline, "%*s %i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i \n", &Vert[0],&UV[0],&Norm[0],&Vert[1],&UV[1],&Norm[1],&Vert[2],&UV[2],&Norm[2],&Vert[3],&UV[3],&Norm[3])==12)
                        {
                                for(unsigned int i=0;i<4;i++)
                                {
                                        Vert[i]--;
                                        UV[i]--;
                                        Norm[i]--;
                                        QuadIndexBuffer.IndexList.push_back(AddVectorIndex(Vert[i],UV[i],Norm[i]));
                                }
                                //Quad Normals
                        }else if(sscanf(oneline, "%*s %i/%i/%i %i/%i/%i %i/%i/%i \n", &Vert[0],&UV[0],&Norm[0],&Vert[1],&UV[1],&Norm[1],&Vert[2],&UV[2],&Norm[2])==9)
                        {
                                for(unsigned int i=0;i<3;i++)
                                {
                                        Vert[i]--;
                                        UV[i]--;
                                        Norm[i]--;
                                        TriIndexBuffer.IndexList.push_back(AddVectorIndex(Vert[i],UV[i],Norm[i]));
                                }
                                //Tri Normals
                        }else if(sscanf(oneline, "%*s %i/%i/%i %i/%i/%i \n", &Vert[0],&UV[0],&Norm[0],&Vert[1],&UV[1],&Norm[1])==6)
                        {
                                for(unsigned int i=0;i<2;i++)
                                {
                                        Vert[i]--;
                                        UV[i]--;
                                        Norm[i]--;
                                        EdgeIndexBuffer.IndexList.push_back(AddVectorIndex(Vert[i],UV[i],Norm[i]));
                                }
                                //Edge Normals
                        }else if(sscanf(oneline, "%*s %i//%i %i//%i %i//%i %i//%i \n", &Vert[0],&Norm[0],&Vert[1],&Norm[1],&Vert[2],&Norm[2],&Vert[3],&Norm[3])==8)
                        {
                                for(unsigned int i=0;i<4;i++)
                                {
                                        Vert[i]--;
                                        UV[i]--;
                                        Norm[i]--;
                                        QuadIndexBuffer.IndexList.push_back(AddVectorIndex(Vert[i],-1,Norm[i]));
                                }
                                //Quad no UV
                        }else if(sscanf(oneline, "%*s %i//%i %i//%i %i//%i \n", &Vert[0],&Norm[0],&Vert[1],&Norm[1],&Vert[2],&Norm[2])==6)
                        {
                                for(unsigned int i=0;i<3;i++)
                                {
                                        Vert[i]--;
                                        UV[i]--;
                                        Norm[i]--;
                                        TriIndexBuffer.IndexList.push_back(AddVectorIndex(Vert[i],-1,Norm[i]));
                                }
                                //Tri no UV
                        }else if(sscanf(oneline, "%*s %i//%i %i//%i \n", &Vert[0],&Norm[0],&Vert[1],&Norm[1])==4)
                        {
                                for(unsigned int i=0;i<2;i++)
                                {
                                        Vert[i]--;
                                        UV[i]--;
                                        Norm[i]--;
                                        EdgeIndexBuffer.IndexList.push_back(AddVectorIndex(Vert[i],-1,Norm[i]));
                                }
                                //Edge no UV
                        }else if(sscanf(oneline, "%*s %i/%i %i/%i %i/%i %i/%i \n", &Vert[0],&UV[0],&Vert[1],&UV[1],&Vert[2],&UV[2],&Vert[3],&UV[3])==8)
                        {
                                for(unsigned int i=0;i<4;i++)
                                {
                                        Vert[i]--;
                                        UV[i]--;
                                        QuadIndexBuffer.IndexList.push_back(AddVectorIndex(Vert[i],UV[i],-1));
                                }
                                //Quad No normals
                        }else if(sscanf(oneline, "%*s %i/%i %i/%i %i/%i \n", &Vert[0],&UV[0],&Vert[1],&UV[1],&Vert[2],&UV[2])==6)
                        {
                                for(int i=0;i<3;i++)
                                {
                                        Vert[i]--;
                                        UV[i]--;
                                        TriIndexBuffer.IndexList.push_back(AddVectorIndex(Vert[i],UV[i],-1));
                                }
                                //Tri No Normals
                        }else if(sscanf(oneline, "%*s %i/%i %i/%i \n", &Vert[0],&UV[0],&Vert[1],&UV[1])==4)
                        {
                                for(int i=0;i<2;i++)
                                {
                                        Vert[i]--;
                                        UV[i]--;
                                        EdgeIndexBuffer.IndexList.push_back(AddVectorIndex(Vert[i],UV[i],-1));
                                }
                                //Edge No Normals
                        }else if(sscanf(oneline, "%*s %i %i %i %i \n", &Vert[0],&Vert[1],&Vert[2],&Vert[3])==4)
                        {
                                for(int i=0;i<4;i++)
                                {
                                        Vert[i]--;
                                        QuadIndexBuffer.IndexList.push_back(AddVectorIndex(Vert[i],-1,-1));
                                }
                                //Quad No UV
                        }else if(sscanf(oneline, "%*s %i %i %i \n", &Vert[0],&Vert[1],&Vert[2])==3)
                        {
                                for(int i=0;i<3;i++)
                                {
                                        Vert[i]--;
                                        TriIndexBuffer.IndexList.push_back(AddVectorIndex(Vert[i],-1,-1));
                                }
                                //Tri No UV
                        }else if(sscanf(oneline, "%*s %i %i \n", &Vert[0],&Vert[1])==2)
                        {
                                for(int i=0;i<2;i++)
                                {
                                        Vert[i]--;
                                        EdgeIndexBuffer.IndexList.push_back(AddVectorIndex(Vert[i],-1,-1));
                                }
                                //Edge No UV
                        }
                        FaceCount=FaceCount++;
                }
                if(!strcmp(readString,"o")||!strcmp(readString,"g"))
                {
// ******************************* New Group (Load Mesh-Group from file) *******
//this section is pretty simple, if we have a new mesh group we just need to store what index that mesh starts
                        Mesh_Component MeshBuf;						//temp meshbuffer

                        MeshBuf.Low_QuadIndex	= QuadIndexBuffer.IndexList.size();
                        MeshBuf.Low_TriIndex	= TriIndexBuffer.IndexList.size();
                        MeshBuf.Low_EdgeIndex	= EdgeIndexBuffer.IndexList.size();

//<<<<<<<<<<<<<<<<<<<<<<<<<<	MeshBuf.Material=CurMat;					//set material

                        if(QuadIndexBuffer.IndexList.size() > 0)
                        {
                                Last_QuadIndex = QuadIndexBuffer.IndexList.size()-1;
                        }
                        else 
                        {
                                        Last_QuadIndex =0;
                        }
                        if(TriIndexBuffer.IndexList.size() > 0)
                        {
                                Last_TriIndex = TriIndexBuffer.IndexList.size()-1;
                        }
                        else
                        {
                                Last_TriIndex =0;
                        }
                        if(EdgeIndexBuffer.IndexList.size() > 0)
                        {
                                Last_EdgeIndex = EdgeIndexBuffer.IndexList.size()-1;
                        }
                        else
                        {
                                Last_EdgeIndex = 0;
                        }
                        if(MeshRef.size()>0)
                        {
                            MeshRef.at(MeshRef.size()-1).QuadIndex = Last_QuadIndex;
                            MeshRef.at(MeshRef.size()-1).TriIndex  = Last_TriIndex;
                            MeshRef.at(MeshRef.size()-1).EdgeIndex = Last_EdgeIndex;
//						MeshRef.at(MeshRef.size()-1).Material = CurMat;
                            MeshRef.at(MeshRef.size()-1).Smoothing=Smoothing;
                        }
                        MeshRef.push_back(MeshBuf);
                }
                readstr(TheMeshFile,oneline);         // Get Single Line Of Data
        }
// ******************************* Clean Section (All data loaded, organise it) 
        //If the loops ended then the file was read fully (EOF)
        //first we end the last mesh
        if(MeshRef.size()>0)
        {//Existing meshes, generate additional
                MeshRef.at(MeshRef.size()-1).QuadIndex = QuadIndexBuffer.IndexList.size();
                MeshRef.at(MeshRef.size()-1).TriIndex  = TriIndexBuffer.IndexList.size();
                MeshRef.at(MeshRef.size()-1).EdgeIndex = EdgeIndexBuffer.IndexList.size();
//			MeshRef.at(MeshRef.size()-1).Material  = CurMat;
                MeshRef.at(MeshRef.size()-1).Smoothing = Smoothing;
        }
        else
        {//no meshes, generate new
                Mesh_Component MeshBuf;						//temp meshbuffer
                MeshBuf.Low_QuadIndex	= 0;
                MeshBuf.Low_TriIndex	= 0;
                MeshBuf.Low_EdgeIndex	= 0;
//			MeshBuf.Material=CurMat;					//set material

                MeshBuf.QuadIndex = QuadIndexBuffer.IndexList.size();
                MeshBuf.TriIndex = TriIndexBuffer.IndexList.size();
                MeshBuf.EdgeIndex = EdgeIndexBuffer.IndexList.size();
                MeshBuf.Smoothing = Smoothing;
                MeshRef.push_back(MeshBuf);

                if(!(MeshRef.size()>0))
                {
                        ERRSYS->Write (25,"OBJ Loader Error","Mesh Reference Size Invalid");
                }
        }
        //Now we take the vertex data and recompile it (using the index list) into an interleaved array
        for(unsigned int a=0;a<Vector_Index_Buffer.size();a++)
        {
                Array_One_Line ThisLine; //this line of the array

                //set index for this line
                ThisLine.Index = Vector_Index_Buffer.at(a).x;

                //we allways have a vertex so we start there
                unsigned int Vert = Vector_Index_Buffer.at(a).y;
                ThisLine.Vertex[0] = VertexBuffer.at(Vert).x;
                ThisLine.Vertex[1] = VertexBuffer.at(Vert).y;
                ThisLine.Vertex[2] = VertexBuffer.at(Vert).z;

                //assign the normal
                unsigned int Normal = Vector_Index_Buffer.at(a).w;
                if(Normal==-1) //if there is no normal;
                {
                        ThisLine.Normal[0]=0;
                        ThisLine.Normal[1]=0;
                        ThisLine.Normal[2]=0;
                }
                else
                {
                        ThisLine.Normal[0] = NormalBuffer.at (Normal).x;
                        ThisLine.Normal[1] = NormalBuffer.at (Normal).y;
                        ThisLine.Normal[2] = NormalBuffer.at (Normal).z;
                }
                unsigned int UV = Vector_Index_Buffer.at(a).z;
                if(UV==-1) //if there is no UV;
                {
                        ThisLine.UV[0]=0;
                        ThisLine.UV[1]=0;
                }
                else
                {
                        ThisLine.UV[0] = UVBuffer.at(UV).x;
                        ThisLine.UV[1] = UVBuffer.at(UV).y;
                }
                ArrayBuffer.push_back(ThisLine);
        }
        //now we can build an actual model array.
        unsigned int Array_Size = ArrayBuffer.size()*3;	//allocate for Verts
        Array_Size = Array_Size + ArrayBuffer.size()*2;	//allocate for UV's
        Array_Size = Array_Size + ArrayBuffer.size()*3;	//allocate for Normals
        ModelArray = new float[Array_Size]; //allocate new array (Modifying for starting at 0)
        for(unsigned int i=0, n=0; i<Array_Size-7;i=i+8)//jump in blocks of 8
        {
                //for each line of the new array we need to add:
                //vertex
                ModelArray[i] = ArrayBuffer.at(n).Vertex[0];
                ModelArray[i+1] = ArrayBuffer.at(n).Vertex[1];
                ModelArray[i+2] = ArrayBuffer.at(n).Vertex[2];
                //Normal
                ModelArray[i+3] = ArrayBuffer.at(n).Normal[0];
                ModelArray[i+4] = ArrayBuffer.at(n).Normal[1];
                ModelArray[i+5] = ArrayBuffer.at(n).Normal[2];
                //UV
                ModelArray[i+6] = ArrayBuffer.at(n).UV[0];
                ModelArray[i+7] = ArrayBuffer.at(n).UV[1];
                n++;
        }
        //We now have an interleaved array with all Vertex, UV, and Normal Data.   (format, | VVVNNNUU | blocks with +8 stride)
        //so now we need an actual model!
        InternalModel = new Base_Model;
        Base_Mesh* MeshBuffer;
        List_Data_leaved<float> * ModelArray_List = new List_Data_leaved<float>;
        unsigned int Stride = sizeof(float)*8;
        ModelArray_List->AssignListData(ModelArray,ArrayBuffer.size(),Stride,true);
        ModelArray_List->SetVertex(0*sizeof(float),3);
        ModelArray_List->SetNormal(3*sizeof(float),3);
        ModelArray_List->SetUV(6*sizeof(float),2);
        unsigned int MeshLimit = MeshRef.size();
        for(unsigned int i = 0; i<MeshLimit; i++)
        {
                //Now we need to generate an actual Model by creating Sub-meshes, these are thus done Per mesh (the loop above)
                //first thing is first we must generate an index array for each Sub-mesh
                unsigned int* QuadIndexArray;
                unsigned int* TriIndexArray;
                unsigned int* EdgeIndexArray;
                unsigned int Q_ArraySize = 0;
                unsigned int T_ArraySize = 0;
                unsigned int E_ArraySize = 0;
                //how big are the arrays?     Highest Number    (minus)    Lowest Number             = Total 
                int Q_ArraySizei = MeshRef.at(i).QuadIndex - MeshRef.at(i).Low_QuadIndex;
                int T_ArraySizei = MeshRef.at(i).TriIndex - MeshRef.at(i).Low_TriIndex;
                int E_ArraySizei = MeshRef.at(i).EdgeIndex - MeshRef.at(i).Low_EdgeIndex;
                //We must disregard invalid sizes (negative values)
                //we also must disregard sizes too small to hold the data (quad=4, etc)
                if(Q_ArraySizei > 3) Q_ArraySize = Q_ArraySizei;
                if(T_ArraySizei > 2) T_ArraySize = T_ArraySizei;
                if(E_ArraySizei > 1) E_ArraySize = E_ArraySizei;
                if(Q_ArraySize > 0) //if there are quads
                {
                        MeshBuffer = new Base_Mesh();
                        if(MeshRef.at(i).Smoothing) MeshBuffer->Type = Base_Mesh::Smooth;
                        else MeshBuffer->Type = Base_Mesh::Solid;

                        QuadIndexArray=new unsigned int[Q_ArraySize];
                        for(unsigned int b=0, c = MeshRef.at(i).Low_QuadIndex; c<MeshRef.at(i).QuadIndex; c++)
                        {
                                QuadIndexArray[b] = QuadIndexBuffer.IndexList.at(c);
                                b++;
                        }
                        //now we have compiled the array we can parse the data to a new quad.
                        MeshBuffer->AssignInterleavedList(ModelArray_List,false); //this array data is shared thus only the last mesh using it will be set to clean
                        List_Data<unsigned int>* IndexList = new List_Data<unsigned int>;
                        IndexList->AssignListData(QuadIndexArray,Q_ArraySize, true);
						IndexList->SetBlockSize(1);
                        MeshBuffer->AssignIndexList(IndexList, true);
                        MeshBuffer->RenderMode = Base_Mesh::Quad;
                        InternalModel->AddMesh(std::auto_ptr<Base_Mesh>(MeshBuffer));
                }
                if(T_ArraySize > 0) //if there are Trianges
                {
                        MeshBuffer = new Base_Mesh();
                        if(MeshRef.at(i).Smoothing) MeshBuffer->Type = Base_Mesh::Smooth;
                        else MeshBuffer->Type = Base_Mesh::Solid;

                        TriIndexArray=new unsigned int[T_ArraySize];
                        for(unsigned int b=0, c = MeshRef.at(i).Low_TriIndex; c<MeshRef.at(i).TriIndex; c++)
                        {
                                TriIndexArray[b] = TriIndexBuffer.IndexList.at(c);
                                b++;
                        }
                        //now we have compiled the array we can parse the data to a new quad.
                        MeshBuffer->AssignInterleavedList(ModelArray_List,false); //this array data is shared thus only the last mesh using it will be set to clean
                        List_Data<unsigned int>* IndexList = new List_Data<unsigned int>;
                        IndexList->AssignListData(TriIndexArray,T_ArraySize, true);
						IndexList->SetBlockSize(1);
                        MeshBuffer->AssignIndexList(IndexList, true);
                        MeshBuffer->RenderMode = Base_Mesh::Tri;
                        InternalModel->AddMesh(std::auto_ptr<Base_Mesh>(MeshBuffer));
                }
                if(E_ArraySize > 0) //if there are Edges
                {
                        MeshBuffer = new Base_Mesh();
                        MeshBuffer->RenderMode = Base_Mesh::Line;
                        MeshBuffer->Type = Base_Mesh::Wire;
                        EdgeIndexArray=new unsigned int[E_ArraySize];
                        for(unsigned int b=0, c = MeshRef.at(i).Low_EdgeIndex; c<MeshRef.at(i).EdgeIndex; c++)
                        {
                                EdgeIndexArray[b] = EdgeIndexBuffer.IndexList.at(c);
                                b++;
                        }
                        //now we have compiled the array we can parse the data to a new quad.
                        MeshBuffer->AssignInterleavedList(ModelArray_List,false); //this array data is shared thus only the last mesh using it will be set to clean
                        List_Data<unsigned int>* IndexList = new List_Data<unsigned int>;
                        IndexList->AssignListData(EdgeIndexArray,E_ArraySize, true);
						IndexList->SetBlockSize(1);
                        MeshBuffer->AssignIndexList(IndexList, true);
                        InternalModel->AddMesh(std::auto_ptr<Base_Mesh>(MeshBuffer));
                }
        }
        //set the first mesh using the interleaved array to clean
        MeshBuffer = *(InternalModel->Meshes.begin());
        MeshBuffer->SetInterleavedClean(true); //pray for no leaks
        fclose(TheMeshFile);
    }else status=false;
    return Add(InternalModel); //A false status may indicate a total faliure or just a glitch
}

unsigned int OBJ_Model::Add(Base_Model* Model)
{
    if(Model == NULL)return 0;
    std::pair<std::map<unsigned int, Base_Model*>::iterator, bool> RetVal;
	std::pair<unsigned int, Base_Model *> Bob;
    RetVal = Models.insert(std::make_pair(CurHigh, Model));
    if(RetVal.second) CurHigh++;
    else return 0;
    unsigned int Number = RetVal.first->first;
    return Number+1;
}
void OBJ_Model::Clear(unsigned int Model)
{
	Model--;
    std::map<unsigned int, Base_Model*>::iterator Loc= Models.find(Model);
    if(Loc != Models.end())
    {
        delete Loc->second;
        Models.erase(Loc);
    }
    return;
}


std::string Write(std::string, Base_Model*)
{
    return "ERROR";
}