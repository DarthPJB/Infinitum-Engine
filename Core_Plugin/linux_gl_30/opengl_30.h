/* 
 * File:   opengl_30.h
 * Author: John Bargman
 *              This is an example plugin providing opengl 3 support for linux
 * Created on 02 September 2011, 23:10
 */

#ifndef OPENGL_30_H
#define	OPENGL_30_H
#define GL_GLEXT_PROTOTYPES     //needed for buffers (apparently)
#include "Plugin_Static.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <GL/glxext.h>

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092

typedef void BYTE;

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

class ShaderManager;

class Render_LINUX_GL_30 : public RenderServer
{
public:
    //Launch Giant robots from the future
    virtual bool init();
	Render_LINUX_GL_30 (PluginManager * han);
    ~Render_LINUX_GL_30();
    virtual void CloseRender();
    virtual bool PreInit();
    virtual void RenderMesh(Base_Mesh *);       //render a mesh
    virtual void RenderModel(Base_Model*);
    virtual void FrameEnd();
    virtual void FrameStart();
    void ReScaleView(int w, int h,bool threeD);
    void ReScaleWindow(int w, int h);
    bool ToggleFullScreen(bool);
    virtual void OptimiseMesh(Base_Mesh *);
    virtual void OptimiseModel(Base_Model *);
    virtual void FreeModelData(Base_Model *);
    virtual void FreeMeshData(Base_Mesh *);
	unsigned int LoadTextureRAW( const char * filename, int wrap );
private:
    Display *display;
    Window win;
    GLXContext ctx;
    Colormap cmap;


	GLint  texture;
	GLint  texture2;
	ShaderManager * Basic;
	glm::mat4 PROJECTION_MAT;
	glm::mat4 MODELVIEW_MAT;

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
    bool isExtensionSupported(const char *extList, const char *extension);
    void reshape2D(GLint w, GLint h);
    void reshape3D(GLint w, GLint h);
	bool fullscreen;
    unsigned int Frame;
};


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

bool ctxErrorOccurred = false;
int ctxErrorHandler( Display *dpy, XErrorEvent *ev );


#endif	/* OPENGL_30_H */