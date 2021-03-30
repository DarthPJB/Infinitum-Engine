/* 
 * File:   opengl_30.h
 * Author: John Bargman
 *              This is an example plugin providing opengl 3 support for Windows
 * Created on 02 September 2011, 23:10 (c) John Bargman
 */

#ifndef OPENGL_30_H
#define	OPENGL_30_H
#include "Plugin_Static.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "GL/glew.h"
#include "GL/wglew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#pragma comment(lib, "glew32.lib")

class ShaderManager;

class Render_Windows_GL_30 : public RenderServer
{
public:
    //Launch Giant robots from the future
    virtual bool init();
	Render_Windows_GL_30(PluginManager * han);
    ~Render_Windows_GL_30();
    virtual void CloseRender();
    virtual bool PreInit();
    virtual void RenderMesh(Base_Mesh *);       //render a mesh
    virtual void RenderModel(Base_Model*, glm::mat4);
    virtual void FrameEnd();
    virtual void FrameStart();
    void ReScaleView(int w, int h,bool threeD);
    void ReScaleWindow(int w, int h);
    bool ToggleFullScreen(bool);
    virtual void OptimiseMesh(Base_Mesh *);
    virtual void OptimiseModel(Base_Model *);
    virtual void FreeModelData(Base_Model *);
    virtual void FreeMeshData(Base_Mesh *);
private:
	virtual void RenderModel(Base_Model*);
	HWND hWnd;
	HDC hDC;
	HGLRC hRC;                        // OpenGL Rendering Context 
	HINSTANCE hInstance;                          // Holds The Instance Of The Application


	GLint  texture;
	GLint  texture2;
	ShaderManager * Basic;
	glm::mat4 PROJECTION_MAT;
	glm::mat4 MODELVIEW_MAT;
	glm::mat4 PROJECTION_MAT_1;
	glm::mat4 MODELVIEW_MAT_1;

	GLint texture_val;
	GLint texture_val2;
	GLint InvTrans;
	GLint Normal;
	GLint Vertex;
	GLint Colour;
	GLint UV;
	GLint Edge;
	GLint Projection;
	GLint Transform;

    bool Currently_Active;
	int OpenGLVersion[2];
    int isExtensionSupported(const char *extension);
    void reshape2D(GLint w, GLint h);
    void reshape3D(GLint w, GLint h);
	bool fullscreen;
    unsigned int Frame;
};

//Tempoary shader management interface (will later possibly be extracted to seperate plugin dependant on DirectX shader requirements)
class ShaderManager		
{
public:
	ShaderManager(ErrorServer * ERRSYS);
	~ShaderManager();
	unsigned int LoadShader(std::string path, bool mode);
	unsigned int LoadGlobalShader(std::string path, bool mode);
	unsigned int MakeProgram(unsigned int* ShaderArray, unsigned int NumberOf);
	void ClearProgram(unsigned int);
	void UseProgram(unsigned int);
private:
	struct S_Program
	{
		std::vector<unsigned int> Used_Shaders;
		unsigned int ShaderProgram;
		bool Linked;
	};
	ErrorServer * ERRSYS;
	unsigned int LoadShaderCode(std::string, bool Mode);
	bool CompileShader(unsigned int);
	std::vector<unsigned int> VertShaders;
	std::vector<unsigned int> FragShaders;
	std::vector<unsigned int> GlobalShaders;
	std::vector<S_Program> ShaderPrograms;
};
	
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);               // Declaration For WndProc
#endif	/* OPENGL_30_H */