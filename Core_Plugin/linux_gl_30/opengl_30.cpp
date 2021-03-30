#include <vector>

#include "opengl_30.h"
#include "soil/SOIL.h"
#include <fstream>
#include <sstream>

PluginData PLF_Init()
{
    PluginData RetVal;
    RetVal.Name = "OpenGL_3_0";
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
    new Render_LINUX_GL_30(handler);
    return true;
}
Render_LINUX_GL_30::Render_LINUX_GL_30(PluginManager * han) : RenderServer(han, "OpenGL_3_0")
{
    Handler->RegisterRender(Name,std::auto_ptr<RenderServer>(this));
    ctx = 0;
    Frame=0;
}
Render_LINUX_GL_30::~Render_LINUX_GL_30()
{
    CloseRender();
}
bool Render_LINUX_GL_30::PreInit()
{
    return true;                //actual checking is needed here, but for now we 'fake it'
}
bool Render_LINUX_GL_30::init()
{
  ERRSYS = Handler->GrabError();
  display = XOpenDisplay(0);
 
  if ( !display )
  {
    printf( "Failed to open X display\n" );
    exit(1);
  }
 
  // Get a matching FB config
  static int visual_attribs[] =
    {
      GLX_X_RENDERABLE    , True,
      GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
      GLX_RENDER_TYPE     , GLX_RGBA_BIT,
      GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
      GLX_RED_SIZE        , 8,
      GLX_GREEN_SIZE      , 8,
      GLX_BLUE_SIZE       , 8,
      GLX_ALPHA_SIZE      , 8,
      GLX_DEPTH_SIZE      , 24,
      GLX_STENCIL_SIZE    , 8,
      GLX_DOUBLEBUFFER    , True,
      //GLX_SAMPLE_BUFFERS  , 1,
      //GLX_SAMPLES         , 4,
      None
    };
 
  int glx_major, glx_minor;
 
  // FBConfigs were added in GLX version 1.3.
  if ( !glXQueryVersion( display, &glx_major, &glx_minor ) || 
       ( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) )
  {
    printf( "Invalid GLX version" );
    exit(1);
  }
 
  printf( "Getting matching framebuffer configs\n" );
  int fbcount;
  GLXFBConfig *fbc = glXChooseFBConfig( display, DefaultScreen( display ), 
                                        visual_attribs, &fbcount );
  if ( !fbc )
  {
    printf( "Failed to retrieve a framebuffer config\n" );
    exit(1);
  }
  printf( "Found %d matching FB configs.\n", fbcount );
 
  // Pick the FB config/visual with the most samples per pixel
  printf( "Getting XVisualInfos\n" );
  int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
 
  int i;
  for ( i = 0; i < fbcount; i++ )
  {
    XVisualInfo *vi = glXGetVisualFromFBConfig( display, fbc[i] );
    if ( vi )
    {
      int samp_buf, samples;
      glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
      glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLES       , &samples  );
 
      printf( "  Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d,"
              " SAMPLES = %d\n", 
              i, vi -> visualid, samp_buf, samples );
 
      if ( best_fbc < 0 || samp_buf && samples > best_num_samp )
        best_fbc = i, best_num_samp = samples;
      if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
        worst_fbc = i, worst_num_samp = samples;
    }
    XFree( vi );
  }
 
  GLXFBConfig bestFbc = fbc[ best_fbc ];
 
  // Be sure to free the FBConfig list allocated by glXChooseFBConfig()
  XFree( fbc );
 
  // Get a visual
  XVisualInfo *vi = glXGetVisualFromFBConfig( display, bestFbc );
  printf( "Chosen visual ID = 0x%x\n", vi->visualid );
 
  printf( "Creating colormap\n" );
  XSetWindowAttributes swa;
  swa.colormap = cmap = XCreateColormap( display,
                                         RootWindow( display, vi->screen ), 
                                         vi->visual, AllocNone );
  swa.background_pixmap = None ;
  swa.border_pixel      = 0;
  swa.event_mask        = StructureNotifyMask;
 
  printf( "Creating window\n" );
  win = XCreateWindow( display, RootWindow( display, vi->screen ), 
                              0, 0, 800, 600, 0, vi->depth, InputOutput, 
                              vi->visual, 
                              CWBorderPixel|CWColormap|CWEventMask, &swa );
  if ( !win )
  {
    printf( "Failed to create window.\n" );
    exit(1);
  }
 
  // Done with the visual info data
  XFree( vi );
 
  XStoreName( display, win, "GL 3.0 Window" );
 
  printf( "Mapping window\n" );
  XMapWindow( display, win );
 
  // Get the default screen's GLX extension list
  const char *glxExts = glXQueryExtensionsString( display,
                                                  DefaultScreen( display ) );
 
  // NOTE: It is not necessary to create or make current to a context before
  // calling glXGetProcAddressARB
  glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
  glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
           glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );

 
  // Install an X error handler so the application won't exit if GL 3.0
  // context allocation fails.
  //
  // Note this error handler is global.  All display connections in all threads
  // of a process use the same error handler, so be sure to guard against other
  // threads issuing X commands while this code is running.
  ctxErrorOccurred = false;
  int (*oldHandler)(Display*, XErrorEvent*) =
      XSetErrorHandler(&ctxErrorHandler);
 
  // Check for the GLX_ARB_create_context extension string and the function.
  // If either is not present, use GLX 1.3 context creation method.
  if ( !isExtensionSupported(glxExts, "GLX_ARB_create_context" ) ||
       !glXCreateContextAttribsARB )
  {
    printf( "glXCreateContextAttribsARB() not found"
            " ... using old-style GLX context\n" );
    ctx = glXCreateNewContext( display, bestFbc, GLX_RGBA_TYPE, 0, True );
  }
 
  // If it does, try to get a GL 3.0 context!
  else
  {
    int context_attribs[] =
      {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
        //GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        None
      };
 
    printf( "Creating context\n" );
    ctx = glXCreateContextAttribsARB( display, bestFbc, 0,
                                      True, context_attribs );
 
    // Sync to ensure any errors generated are processed.
    XSync( display, False );
    if ( !ctxErrorOccurred && ctx )
      printf( "Created GL 3.0 context\n" );
    else
    {
      // Couldn't create GL 3.0 context.  Fall back to old-style 2.x context.
      // When a context version below 3.0 is requested, implementations will
      // return the newest context version compatible with OpenGL versions less
      // than version 3.0.
      // GLX_CONTEXT_MAJOR_VERSION_ARB = 1
      context_attribs[1] = 1;
      // GLX_CONTEXT_MINOR_VERSION_ARB = 0
      context_attribs[3] = 0;
 
      ctxErrorOccurred = false;
 
      printf( "Failed to create GL 3.0 context"
              " ... using old-style GLX context\n" );
      ctx = glXCreateContextAttribsARB( display, bestFbc, 0, 
                                        True, context_attribs );
    }
  }
 
  // Sync to ensure any errors generated are processed.
  XSync( display, False );
 
  // Restore the original error handler
  XSetErrorHandler( oldHandler );
 
  if ( ctxErrorOccurred || !ctx )
  {
    printf( "Failed to create an OpenGL context\n" );
    exit(1);
  }
 
  // Verifying that context is a direct context
  if ( ! glXIsDirect ( display, ctx ) )
  {
    printf( "Indirect GLX rendering context obtained\n" );
  }
  else
  {
    printf( "Direct GLX rendering context obtained\n" );
  }
 
  printf( "Making context current\n" );
  glXMakeCurrent( display, win, ctx );
  ReScaleView(800,800,true);
  Currently_Active = true;
  
  
  	//init
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glClearColor(0.1f, 0.1f, 0.1f, 0.5f);				// Black Background

	Basic = new ShaderManager(ERRSYS);
	unsigned int vert = Basic->LoadGlobalShader("minimal.vert", true);
	unsigned int frag = Basic->LoadGlobalShader("minimal.frag", false);
	//texture = LoadTextureRAW( "texture.raw", TRUE );
	texture = SOIL_load_OGL_texture
	(
		"texture.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS |SOIL_FLAG_TEXTURE_REPEATS| SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
	texture2 = SOIL_load_OGL_texture
	(
		"background.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS |SOIL_FLAG_TEXTURE_REPEATS| SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
	glEnable(GL_TEXTURE_2D);

	unsigned int programe;
	programe = Basic->MakeProgram(NULL,0);
	Basic->UseProgram(programe);
	Normal = glGetAttribLocation(programe, "vNormal");
	Vertex = glGetAttribLocation(programe, "vPosition");
	Colour = glGetAttribLocation(programe, "vColor");
	UV = glGetAttribLocation(programe, "vTexCoord");
	Projection = glGetUniformLocation(programe, "vProject");
	Transform = glGetUniformLocation(programe, "vModelview");
	InvTrans = glGetUniformLocation(programe, "vTranspose");
	texture_val = glGetUniformLocation(programe, "vTexture");
	texture_val2 = glGetUniformLocation(programe, "vTexture2");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texture_val,0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniform1i(texture_val2,1);

}
void Render_LINUX_GL_30::CloseRender()
{
    if(Currently_Active)
    {
        glXMakeCurrent( display, 0, 0 );
        glXDestroyContext( display, ctx );

        XDestroyWindow( display, win );
        XFreeColormap( display, cmap );
        XCloseDisplay( display );
    }
    return;
}
void Render_LINUX_GL_30::RenderModel(Base_Model* Model)
{
    if(Model != NULL)
    {
        for(std::vector<Base_Mesh*>::iterator Cur_Mesh=Model->Meshes.begin(); Cur_Mesh < Model->Meshes.end(); Cur_Mesh++)
        {
            RenderMesh(*Cur_Mesh);
        }
    }
}
void Render_LINUX_GL_30::RenderMesh(Base_Mesh* Mesh)
{
   switch(Mesh->Type)
    {
        case Base_Mesh::Wire:
            glShadeModel(GL_FLAT);
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            break;
        case Base_Mesh::Smooth:
            glShadeModel(GL_SMOOTH);
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            break;
        case Base_Mesh::Solid:
            glShadeModel(GL_FLAT);
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            break;
        case (Base_Mesh::NON_RENDER):
            return;
            break;
        default:
            glShadeModel(GL_FLAT);
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            break;
    }
	
	//@TODO transform here

	glUniformMatrix4fv(Projection, 1, GL_FALSE, &PROJECTION_MAT[0][0]);
	glUniformMatrix4fv(Transform, 1, GL_FALSE, &MODELVIEW_MAT[0][0]);
	glm::mat4 InverseTranspose = MODELVIEW_MAT * PROJECTION_MAT;
	InverseTranspose = glm::inverse(glm::transpose(InverseTranspose));
	glUniformMatrix4fv(InvTrans, 1, GL_FALSE, &InverseTranspose[0][0]);

    if(Mesh->SpecialUsed) 
    {
        if(Mesh->SpecialIndex1 != 0)
        {       //VBO
            
            if(Mesh->SpecialIndex2 != 0)
            {   // VBA
                List_Data_leaved<float> * InterLeaved = Mesh->InterleavedUsed(); 
                List_Data<unsigned int> * IndexList = Mesh->IndexUsed();
                List_Data<float> * VertexList = Mesh->VertexUsed();
                unsigned int NumElements=0;
                if(IndexList != NULL)
                {
                    NumElements = IndexList->GetNumElements();
                }
                else
                {
                    if(InterLeaved != NULL)
                    {
                        if(InterLeaved->CurrentlyUsed('v'))
                        {
                            NumElements = InterLeaved->GetNumElements();
                        }else NumElements = VertexList->GetNumElements();
                    }else NumElements = VertexList->GetNumElements();
                }
                unsigned int VAO = (unsigned int)Mesh->SpecialIndex2;
                //Now to fill in the data, starting with the easy ones.
                glBindVertexArray(VAO);
                //glEnableClientState(GL_VERTEX_ARRAY);
                if(IndexList != NULL)
                {
                    switch (Mesh->RenderMode)
                    {
                        case Base_Mesh::Tri:
                            glDrawElements(GL_TRIANGLES,NumElements, GL_UNSIGNED_INT, (GLvoid*)NULL);
                            break;
                        case Base_Mesh::TriStrip:
                            glDrawElements(GL_TRIANGLE_STRIP,NumElements, GL_UNSIGNED_INT, (GLvoid*)NULL);
                            break;
                        case Base_Mesh::Quad:
                            glDrawElements(GL_QUADS,NumElements, GL_UNSIGNED_INT, (GLvoid*)NULL);
                            break;
                        case Base_Mesh::QuadStrip:
                            glDrawElements(GL_QUAD_STRIP,NumElements, GL_UNSIGNED_INT, (GLvoid*)NULL);
                            break;
                        case Base_Mesh::Polygon:
                            glDrawElements(GL_POLYGON,NumElements, GL_UNSIGNED_INT, (GLvoid*)NULL);
                            break;
                        case Base_Mesh::Line:
                            glDrawElements(GL_LINE,NumElements, GL_UNSIGNED_INT, (GLvoid*)NULL);
                            break;
                        case Base_Mesh::LineStrip:
                            glDrawElements(GL_LINES,NumElements, GL_UNSIGNED_INT, (GLvoid*)NULL);
                            break;
                        case Base_Mesh::Other_Render:
                        default:
                            break;
                    }
                }
                else
                {
                    switch (Mesh->RenderMode)
                    {
                        case Base_Mesh::Tri:
                            glDrawArrays(GL_TRIANGLES,0,NumElements);
                            break;
                        case Base_Mesh::TriStrip:
                            glDrawArrays(GL_TRIANGLE_STRIP,0,NumElements);
                            break;
                        case Base_Mesh::Quad:
                            glDrawArrays(GL_QUADS,0,NumElements);
                            break;
                        case Base_Mesh::QuadStrip:
                            glDrawArrays(GL_QUAD_STRIP,0,NumElements);
                            break;
                        case Base_Mesh::Polygon:
                            glDrawArrays(GL_POLYGON,0,NumElements);
                            break;
                        case Base_Mesh::Line:
                            glDrawArrays(GL_LINE,0,NumElements);
                            break;
                        case Base_Mesh::LineStrip:
                            glDrawArrays(GL_LINES,0,NumElements);
                            break;
                        case Base_Mesh::Other_Render:
                        default:
                            break;
                    }
                }  
            }
            else
            {   //No VBA
                
            }
        }
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        //we would render the HW special here
        return;
    }
    //grab the list pointers
    List_Data<float> * VertexList = Mesh->VertexUsed();
    List_Data<float> * ColourList = Mesh->ColourUsed();
    List_Data<float> * NormalList = Mesh->NormalUsed();
    List_Data<unsigned int> * IndexList = Mesh->IndexUsed();
    List_Data<bool> * EdgeList = Mesh->EdgeUsed();
    List_Data<float> * UVList = Mesh->UVUsed();
    List_Data_leaved<float> * InterLeaved = Mesh->InterleavedUsed(); 
    //assign boolean values for index use
    bool InterleavedUsed = (InterLeaved != NULL)? true : false;
    bool VertexUsed;
    bool ColourUsed;
    bool NormalUsed;
    bool UVUsed;
    if(InterleavedUsed)
    {
        VertexUsed = (InterLeaved->CurrentlyUsed('v') || VertexList!= NULL)? true: false;
        ColourUsed = (InterLeaved->CurrentlyUsed('c') || ColourList!= NULL)? true: false;
        NormalUsed = (InterLeaved->CurrentlyUsed('n') || NormalList!= NULL)? true: false;
        UVUsed = (InterLeaved->CurrentlyUsed('u') || UVList!= NULL)? true:false;
    }
    else
    {
        VertexUsed = (VertexList!= NULL)? true: false;
        ColourUsed = (ColourList!= NULL)? true: false;
        NormalUsed = (NormalList!= NULL)? true: false;
        UVUsed = (UVList!= NULL)? true:false;
    }
    bool IndexUsed = (IndexList != NULL)? true: false;
    bool EdgeUsed = (EdgeList != NULL)? true: false;
    
    if(VertexUsed || InterleavedUsed)
    {        //You can't draw without any vertex data
        if(InterleavedUsed && (!VertexUsed))
        {        //You can't draw without any vertex data
            if(!InterLeaved->CurrentlyUsed('v'))return;
        }
        //glEnableClientState(GL_VERTEX_ARRAY);
        if(VertexUsed)
        {
            if(InterleavedUsed)
            {
				if(InterLeaved->CurrentlyUsed('v'))
				{
					glVertexAttribPointer(Vertex, InterLeaved->GetBlockSize('v'), GL_FLOAT, GL_FALSE, InterLeaved->GetStride(), InterLeaved->GetListData('v'));
					glEnableVertexAttribArray(Vertex);
				}
				else 
				{
					glVertexAttribPointer(Vertex, VertexList->GetBlockSize(), GL_FLOAT, GL_FALSE, VertexList->GetStride(), VertexList->GetListData());
					glEnableVertexAttribArray(Vertex);
				}
            }
			else
			{
				glVertexAttribPointer(Vertex, VertexList->GetBlockSize(), GL_FLOAT, GL_FALSE, VertexList->GetStride(), VertexList->GetListData());
				glEnableVertexAttribArray(Vertex);
			}
        }

        if(ColourUsed)
        {
            //glEnableClientState(GL_COLOR_ARRAY);
            if(InterleavedUsed)
            {
				if(InterLeaved->CurrentlyUsed('c'))
				{
					glVertexAttribPointer(Colour, InterLeaved->GetBlockSize('c'), GL_FLOAT, GL_FALSE, InterLeaved->GetStride(), InterLeaved->GetListData('c'));
					glEnableVertexAttribArray(Colour);
				}
				else 
				{
					glVertexAttribPointer(Colour, ColourList->GetBlockSize(), GL_FLOAT, GL_FALSE, ColourList->GetStride(), ColourList->GetListData());
					glEnableVertexAttribArray(Colour);
				}
            }
			else
			{
				glVertexAttribPointer(Colour, ColourList->GetBlockSize(), GL_FLOAT, GL_FALSE, ColourList->GetStride(), ColourList->GetListData());
				glEnableVertexAttribArray(Colour);
			}
        }
        if(NormalUsed)
        {
            //glEnableClientState(GL_NORMAL_ARRAY);
            if(InterleavedUsed)
            {
				if(InterLeaved->CurrentlyUsed('n'))
				{
					if(Normal!= -1)
					{
						glVertexAttribPointer(Normal, InterLeaved->GetBlockSize('n'), GL_FLOAT, GL_FALSE, InterLeaved->GetStride(), InterLeaved->GetListData('n'));
						glEnableVertexAttribArray(Normal);
					}
				}
				else 
				{
					if(Normal!= -1)
					{
						glVertexAttribPointer(Normal, NormalList->GetBlockSize(), GL_FLOAT, GL_FALSE, NormalList->GetStride(), NormalList->GetListData());
						glEnableVertexAttribArray(Normal);
					}
				}
            }
			else
			{
				if(Normal!= -1)
				{
					glVertexAttribPointer(Normal, NormalList->GetBlockSize(), GL_FLOAT, GL_FALSE, NormalList->GetStride(), NormalList->GetListData());
					glEnableVertexAttribArray(Normal);
				}
			}
        }
        if(UVUsed)
        {
            //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            if(InterleavedUsed)
            {
				if(InterLeaved->CurrentlyUsed('u'))
				{
					glVertexAttribPointer(3, InterLeaved->GetBlockSize('u'), GL_FLOAT, GL_FALSE, InterLeaved->GetStride(), InterLeaved->GetListData('u'));
					if(UV!= -1)glEnableVertexAttribArray(UV);
				}
				else 
				{
					glVertexAttribPointer(3, UVList->GetBlockSize(), GL_FLOAT, GL_FALSE, UVList->GetStride(), UVList->GetListData());
					if(UV!= -1)glEnableVertexAttribArray(UV);
				}
            }
			else
			{
				glVertexAttribPointer(3, UVList->GetBlockSize(), GL_FLOAT, GL_FALSE, UVList->GetStride(), UVList->GetListData());
				if(UV!= -1)glEnableVertexAttribArray(UV);
			}
        }
        if(EdgeUsed)
        {
			glVertexAttribPointer(4, EdgeList->GetBlockSize(), GL_BOOL, GL_FALSE,EdgeList->GetStride(), EdgeList->GetListData());
			if(Edge!= -1)glEnableVertexAttribArray(Edge);
        }
        int NumElements=0;
        //Drawing non-indexed list (Plough though in order)
        if(IndexUsed)
        {
            NumElements = IndexList->GetNumElements();
        }
        else
        {
            if(InterLeaved->CurrentlyUsed('v'))
            {
                NumElements = InterLeaved->GetNumElements();
            }else
            {
                NumElements = VertexList->GetNumElements();
            }
        }
        if(!IndexUsed)
        {
            switch (Mesh->RenderMode)
            {
                case Base_Mesh::Tri:
                    glDrawArrays(GL_TRIANGLES,0,NumElements);
                    break;
                case Base_Mesh::TriStrip:
                    glDrawArrays(GL_TRIANGLE_STRIP,0,NumElements);
                    break;
                case Base_Mesh::Quad:
                    glDrawArrays(GL_QUADS,0,NumElements);
                    break;
                case Base_Mesh::QuadStrip:
                    glDrawArrays(GL_QUAD_STRIP,0,NumElements);
                    break;
                case Base_Mesh::Polygon:
                    glDrawArrays(GL_POLYGON,0,NumElements);
                    break;
                case Base_Mesh::Line:
                    glDrawArrays(GL_LINE,0,NumElements);
                    break;
                case Base_Mesh::LineStrip:
                    glDrawArrays(GL_LINES,0,NumElements);
                    break;
                case Base_Mesh::Other_Render:
                default:
                    break;
            }
        }
        else
        {
            switch (Mesh->RenderMode)
            {
                case Base_Mesh::Tri:
                    glDrawElements(GL_TRIANGLES,NumElements, GL_UNSIGNED_INT, IndexList->GetListData());
                    break;
                case Base_Mesh::TriStrip:
                    glDrawElements(GL_TRIANGLE_STRIP,NumElements, GL_UNSIGNED_INT, IndexList->GetListData());
                    break;
                case Base_Mesh::Quad:
                    glDrawElements(GL_QUADS,NumElements, GL_UNSIGNED_INT, IndexList->GetListData());
                    break;
                case Base_Mesh::QuadStrip:
                    glDrawElements(GL_QUAD_STRIP,NumElements, GL_UNSIGNED_INT, IndexList->GetListData());
                    break;
                case Base_Mesh::Polygon:
                    glDrawElements(GL_POLYGON,NumElements, GL_UNSIGNED_INT, IndexList->GetListData());
                    break;
                case Base_Mesh::Line:
                    glDrawElements(GL_LINE,NumElements, GL_UNSIGNED_INT, IndexList->GetListData());
                    break;
                case Base_Mesh::LineStrip:
                    glDrawElements(GL_LINES,NumElements, GL_UNSIGNED_INT, IndexList->GetListData());
                    break;
                case Base_Mesh::Other_Render:
                default:
                    break;
            }
        }      
    }
    return;
}

void Render_LINUX_GL_30::reshape2D(GLint w, GLint h)
{
	PROJECTION_MAT = glm::ortho(0.0f, (float)w, 0.0f, (float)h);
    return;
}
void Render_LINUX_GL_30::reshape3D(GLint w, GLint h)
{
	PROJECTION_MAT = glm::frustum(-1.0,1.0,-1.0,1.0,1.0,20.0);
    return;
}
void Render_LINUX_GL_30::ReScaleView(int w, int h,bool threeD)
{
	glEnable(GL_DEPTH_TEST);
    glViewport(0,0,(GLsizei)w,(GLsizei)h);
    if(threeD) reshape3D((GLint)w,(GLint)h);
    else reshape2D((GLint)w,(GLint)h);
}

void Render_LINUX_GL_30::ReScaleWindow(int w, int h)
{
    return;            //<<<<<<< This needs implimenting
}
bool Render_LINUX_GL_30::ToggleFullScreen(bool)
{
    return false;       //<<<<<<< This needs implimenting
}

void Render_LINUX_GL_30::FrameEnd()
{
    glFlush();
    glXSwapBuffers ( display, win );
    return;
}
void Render_LINUX_GL_30::FrameStart()
{
    Frame++;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	MODELVIEW_MAT = glm::mat4();
	MODELVIEW_MAT = glm::translate(MODELVIEW_MAT, glm::vec3(0,0,-6));
	MODELVIEW_MAT = glm::rotate(MODELVIEW_MAT, (float)Frame/10, glm::vec3(0.5,1,0));
    return;
}
int ctxErrorHandler( Display *dpy, XErrorEvent *ev )
{
    ctxErrorOccurred = true;
    return 0;
}
bool Render_LINUX_GL_30::isExtensionSupported(const char *extList, const char *extension)
{

  const char *start;
  const char *where, *terminator;
 
  /* Extension names should not have spaces. */
  where = strchr(extension, ' ');
  if ( where || *extension == '\0' )
    return false;

  /* It takes a bit of care to be fool-proof about parsing the
     OpenGL extensions string. Don't be fooled by sub-strings,
     etc. */
  for ( start = extList; ; ) {
    where = strstr( start, extension );
 
    if ( !where )
      break;
 
    terminator = where + strlen( extension );
 
    if ( where == start || *(where - 1) == ' ' )
      if ( *terminator == ' ' || *terminator == '\0' )
        return true;
 
    start = terminator;
  }
 
  return false;
}
void Render_LINUX_GL_30::OptimiseModel(Base_Model * Model)
{
    //call model opt
    Model->Optimise();  //also calls mesh opt, not needed later
    for(std::vector<Base_Mesh*>::iterator touch_my_little_tinkle = Model->Meshes.begin(); touch_my_little_tinkle < Model->Meshes.end(); touch_my_little_tinkle++)
    {
        if((*touch_my_little_tinkle)!=NULL)OptimiseMesh((*touch_my_little_tinkle));
    }
    //could do whole-model opt here (if we wanted)
 return;   
}
void Render_LINUX_GL_30::FreeModelData(Base_Model * Model)
{
    for(std::vector<Base_Mesh*>::iterator touch_my_little_tinkle = Model->Meshes.begin(); touch_my_little_tinkle < Model->Meshes.end(); touch_my_little_tinkle++)
    {
        if((*touch_my_little_tinkle)!=NULL)FreeMeshData((*touch_my_little_tinkle));
    }    
}
void Render_LINUX_GL_30::FreeMeshData(Base_Mesh * Mesh)
{
    if(Mesh->SpecialIndex3 > 0)
    {
        glDeleteBuffers(Mesh->SpecialIndex3,(GLuint*)Mesh->SpecialIndex1);
        delete [] (GLuint*)Mesh->SpecialIndex1;
        glDeleteVertexArrays(1,(GLuint*)&Mesh->SpecialIndex2);
    }
}
void Render_LINUX_GL_30::OptimiseMesh(Base_Mesh * Mesh)
{
    //no need to call Mesh->Optimise() as Model has done this previously (if it hasn't.. Not our fault)
    /* Special1 will be used to hold an array of vertex buffer objects*/
    /* Special2 will be used to hold a single vertex array object*/
    /* Special3 will be used to declare if optimised*/
    //grab the list pointers
    List_Data<float> * VertexList = Mesh->VertexUsed();
    List_Data<float> * ColourList = Mesh->ColourUsed();
    List_Data<float> * NormalList = Mesh->NormalUsed();
    List_Data<unsigned int> * IndexList = Mesh->IndexUsed();
    List_Data<bool> * EdgeList = Mesh->EdgeUsed();
    List_Data<float> * UVList = Mesh->UVUsed();
    List_Data_leaved<float> * InterLeaved = Mesh->InterleavedUsed(); 
    //assign boolean values for index use
    bool InterleavedUsed = (InterLeaved != NULL)? true : false;
    bool IndexUsed = (IndexList != NULL)? true: false;
    bool EdgeUsed = (EdgeList != NULL)? true: false;
    bool VertexUsed = (VertexList!= NULL)? true: false;
    bool ColourUsed = (ColourList!= NULL)? true: false;
    bool NormalUsed = (NormalList!= NULL)? true: false;
    bool UVUsed = (UVList!= NULL)? true:false;
    bool leaved_VertexUsed=false;
    bool leaved_ColourUsed=false;
    bool leaved_NormalUsed=false;
    bool leaved_UVUsed=false;
    if(InterleavedUsed)
    {
        leaved_VertexUsed = (InterLeaved->CurrentlyUsed('v'))? true: false;
        leaved_ColourUsed = (InterLeaved->CurrentlyUsed('c'))? true: false;
        leaved_NormalUsed = (InterLeaved->CurrentlyUsed('n'))? true: false;
        leaved_UVUsed = (InterLeaved->CurrentlyUsed('u'))? true:false;
    }
    unsigned int VBOArraysize =0;
    //only need one VBO for each used array
    if((!leaved_VertexUsed) && VertexUsed) VBOArraysize++;
    if((!leaved_ColourUsed) &&  ColourUsed) VBOArraysize++;
    if((!leaved_NormalUsed) &&  NormalUsed) VBOArraysize++;
    if((!leaved_UVUsed) && UVUsed) VBOArraysize++;
    if(InterleavedUsed) VBOArraysize++;
    if(EdgeUsed) VBOArraysize++;
    if(IndexUsed) VBOArraysize++;
    Mesh->SpecialIndex1 = (unsigned long int) new GLuint[VBOArraysize];
    Mesh->SpecialIndex3 = VBOArraysize;
    Mesh->SpecialUsed = true;
    unsigned int VAO;
    
    glGenBuffers(VBOArraysize, (GLuint*)Mesh->SpecialIndex1);
    glGenVertexArrays(1,&VAO);
    Mesh->SpecialIndex2 = (unsigned long int) VAO;
    
    //Now to fill in the data, starting with the easy ones.
    glBindVertexArray(VAO);
    unsigned int BufferIndex=0;
    GLuint * Buffers = (GLuint*)Mesh->SpecialIndex1;
    if(EdgeUsed)
    {
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[BufferIndex]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(bool)*EdgeList->GetNumElements(), EdgeList->GetListData(), GL_STATIC_DRAW);
		if(Edge != -1)
		{
			glVertexAttribPointer(Edge, 1, GL_BOOL, GL_FALSE,EdgeList->GetStride(), (BYTE*)NULL);
			glEnableVertexAttribArray(Edge);
		}
        BufferIndex++;
    }
    if((!leaved_VertexUsed) && VertexUsed)
    {
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[BufferIndex]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*VertexList->GetNumElements()*VertexList->GetBlockSize(), VertexList->GetListData(), GL_STATIC_DRAW);
		if(Vertex!=-1)
		{
			glVertexAttribPointer(Vertex, 3, GL_FLOAT, GL_FALSE,VertexList->GetStride(), (BYTE*)NULL);
			glEnableVertexAttribArray(Vertex);
		}
        BufferIndex++;
    }
    if((!leaved_ColourUsed) &&  ColourUsed)
    {
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[BufferIndex]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*ColourList->GetNumElements()*ColourList->GetBlockSize(), ColourList->GetListData(), GL_STATIC_DRAW);
		if(Colour!=-1)
		{
			glVertexAttribPointer(Colour, 3, GL_FLOAT, GL_FALSE,ColourList->GetStride(), (BYTE*)NULL);
			glEnableVertexAttribArray(Colour);
		}
        BufferIndex++;
    }
    if((!leaved_NormalUsed) &&  NormalUsed)
    {
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[BufferIndex]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*NormalList->GetNumElements()*NormalList->GetBlockSize(), NormalList->GetListData(), GL_STATIC_DRAW);
		if(Normal!= -1)
		{
			glVertexAttribPointer(Normal, 3, GL_FLOAT, GL_FALSE,NormalList->GetStride(), (BYTE*)NULL);
			glEnableVertexAttribArray(Normal);
		}
        BufferIndex++;
    }
    if((!leaved_UVUsed) && UVUsed)
    {
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[BufferIndex]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*UVList->GetNumElements()*UVList->GetBlockSize(), UVList->GetListData(), GL_STATIC_DRAW);
		if(UV!= -1)
		{
			glVertexAttribPointer(UV, 2, GL_FLOAT, GL_FALSE,UVList->GetStride(), (BYTE*)NULL);
			glEnableVertexAttribArray(UV);
		}
        BufferIndex++;
    }
    if(InterleavedUsed)
    {
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[BufferIndex]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*InterLeaved->GetNumElements()*InterLeaved->GetBlockSize(), InterLeaved->GetListData(), GL_STATIC_DRAW);
        BufferIndex++;
        //now comes the fun part -_-;
        if(leaved_VertexUsed && Vertex!=-1)
        {
			glEnableVertexAttribArray(Vertex);
			glVertexAttribPointer(Vertex, InterLeaved->GetBlockSize('v'), GL_FLOAT, GL_FALSE, InterLeaved->GetStride(), (BYTE*)NULL + InterLeaved->GetBlockOffset('v'));
        }
        if(leaved_ColourUsed && Colour!=-1)
        {
			glEnableVertexAttribArray(Colour);
			glVertexAttribPointer(Colour, InterLeaved->GetBlockSize('c'), GL_FLOAT, GL_FALSE, InterLeaved->GetStride(), (BYTE*)NULL + InterLeaved->GetBlockOffset('c'));
        }
        if(leaved_NormalUsed && Normal!= -1)
        {
			glEnableVertexAttribArray(Normal);
			glVertexAttribPointer(Normal, InterLeaved->GetBlockSize('n'), GL_FLOAT, GL_FALSE, InterLeaved->GetStride(), (BYTE*)NULL + InterLeaved->GetBlockOffset('n'));
        } 
        if(leaved_UVUsed && UV!= -1) 
        {
			glEnableVertexAttribArray(UV);
			//glVertexAttribPointer(UV, 2, GL_FLOAT, GL_FALSE,UVList->GetStride(), (BYTE*)NULL);
			glVertexAttribPointer(UV, InterLeaved->GetBlockSize('u'), GL_FLOAT, GL_FALSE, InterLeaved->GetStride(), (BYTE*)NULL + InterLeaved->GetBlockOffset('u'));
        }
    }
    if(IndexUsed)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,Buffers[BufferIndex]);
		unsigned int BlockSize = IndexList->GetBlockSize();
		unsigned int sizeofVar = sizeof(unsigned int);
		unsigned int EleCount = IndexList->GetNumElements();
		unsigned int Size = EleCount * BlockSize;
		unsigned int Tot = Size * sizeofVar;
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, Tot, IndexList->GetListData(), GL_STATIC_DRAW);
        BufferIndex++;
    }
    glBindVertexArray(0);
	/*
if(InterleavedUsed)
{
	float * BlockStart = InterLeaved->GetListData();
	std::stringstream Kitten;
	for(int Value = 0; Value < InterLeaved->GetNumElements(); Value++)
	{
		Kitten << "Data set:" << Value << "| ";

		int lazor = Value * InterLeaved->GetBlockSize();
		unsigned int OffN = InterLeaved->GetBlockOffset('n') ;
		unsigned int OffV = InterLeaved->GetBlockOffset('v') ;
		unsigned int OffC = InterLeaved->GetBlockOffset('c');
		unsigned int OffU = InterLeaved->GetBlockOffset('u') ;
		for(int I=0; I<InterLeaved->GetBlockSize();I++)
		{
			if(I==OffN)Kitten << "|N|";
			if(I==OffV)Kitten << "|V|";
			if(I==OffC)Kitten << "|C|";
			if(I==OffU)Kitten << "|U|";
			Kitten << BlockStart[lazor+I];
			//else Kitten << ',';
		}
		Kitten << '\n';

	}
	ERRSYS->Write(50,"Test dump of co-ordinates",Kitten.str());
}*/
}

ShaderManager::ShaderManager(ErrorServer * ERRSYS_var)
{
	VertShaders.clear();
	FragShaders.clear();
	ERRSYS = ERRSYS_var;
}

ShaderManager::~ShaderManager()
{
	for(std::vector<unsigned int>::iterator Object = VertShaders.begin(); Object < VertShaders.end(); Object++)
	{
		glDeleteShader(*Object);
	}
	for(std::vector<unsigned int>::iterator Object = FragShaders.begin(); Object < FragShaders.end(); Object++)
	{
		glDeleteShader(*Object);
	}
	for(std::vector<unsigned int>::iterator Object = GlobalShaders.begin(); Object < GlobalShaders.end(); Object++)
	{
		glDeleteShader(*Object);
	}
	for(std::vector<S_Program>::iterator Object = ShaderPrograms.begin(); Object < ShaderPrograms.end(); Object++)
	{
		glDeleteProgram(Object->ShaderProgram);
	}
}
unsigned int ShaderManager::LoadShaderCode(std::string source, bool Mode)
{
	unsigned int Shader = 0;
	if(Mode == true) Shader = glCreateShader(GL_VERTEX_SHADER);
	else Shader = glCreateShader(GL_FRAGMENT_SHADER);

	GLchar const * StringSpace = source.c_str();
	glShaderSource(Shader, 1, & StringSpace, NULL);
	if(!CompileShader(Shader))
	{
		glDeleteShader(Shader);
		return 0;
	}
	else 
	{
		return Shader;
	}
}
bool ShaderManager::CompileShader(unsigned int Shader)
{
	glCompileShader(Shader);
	GLint Param = 0;
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &Param);
	if(Param == GL_TRUE)
	{
		return true;
	}
	else
	{
		char Buffer[250];
		GLint Len = 240;
		glGetShaderInfoLog(Shader,240, &Len, &Buffer[0]);
		if(ERRSYS!=NULL)ERRSYS->Write(50,"Shader Compile Failed output:", std::string(Buffer));
		return false;
	}
}

unsigned int ShaderManager::MakeProgram(unsigned int* ShaderArray, unsigned int NumberOf)
{
	S_Program Shader;
	Shader.ShaderProgram = glCreateProgram();

	for(std::vector<unsigned int>::iterator Object = GlobalShaders.begin(); Object < GlobalShaders.end(); Object++)
	{
		glAttachShader(Shader.ShaderProgram, *Object);
	}

	for(unsigned int Num = 0; Num < NumberOf; Num++)
	{
		Shader.Used_Shaders.push_back(ShaderArray[Num]);
	}

	for(std::vector<unsigned int>::iterator Object = Shader.Used_Shaders.begin(); Object < Shader.Used_Shaders.end(); Object++)
	{
		glAttachShader(Shader.ShaderProgram, *Object);
	}
	glLinkProgram(Shader.ShaderProgram);

	GLint Param = 0;
	glGetProgramiv(Shader.ShaderProgram, GL_LINK_STATUS, &Param);
	if(Param == GL_TRUE)
	{
		Shader.Linked = true;
	}
	else
	{
		Shader.Linked = false;
		char Buffer[250];
		GLint Len = 240;
		glGetProgramInfoLog(Shader.ShaderProgram,240, &Len, &Buffer[0]);
		if(ERRSYS!=NULL)ERRSYS->Write(50,"Shader Linking Failed output:", std::string(Buffer));
	}
	ShaderPrograms.push_back(Shader);
	return Shader.ShaderProgram;
}
void ShaderManager::UseProgram(unsigned int ShaderProgram)
{
	glUseProgram(ShaderProgram);
}
unsigned int ShaderManager::LoadShader(std::string path, bool mode)
{
	std::ifstream ifs(path.c_str());    
	std::string Source((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	unsigned int Shader = LoadShaderCode(Source, mode);
	if(Shader !=0)
	{
		if(mode == true) VertShaders.push_back(Shader);
		else FragShaders.push_back(Shader);
	}
	return Shader;
}
unsigned int ShaderManager::LoadGlobalShader(std::string path, bool mode)
{
	std::ifstream ifs(path.c_str());    
	std::string Source((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	unsigned int Shader = LoadShaderCode(Source, mode);
	if(Shader !=0)
	{
		GlobalShaders.push_back(Shader);
	}
	return Shader;
}

// load a 256x256 RGB .RAW file as a texture
unsigned int Render_LINUX_GL_30::LoadTextureRAW( const char * filename, int wrap )
{
    GLuint texture;
    int width, height;
    BYTE * data;
    FILE * file;

    // open texture data
    file = fopen( filename, "rb" );
    if ( file == NULL ) return 0;

    // allocate buffer
    width = 256;
    height = 256;
    data = (BYTE*) malloc( width * height * 3 );

    // read texture data
    fread( data, width * height * 3, 1, file );
    fclose( file );

    // allocate a texture name
    glGenTextures( 1, &texture );

    // select our current texture
    glBindTexture( GL_TEXTURE_2D, texture );

    // select modulate to mix texture with color for shading
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                     GL_LINEAR_MIPMAP_NEAREST );
    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                     wrap ? GL_REPEAT : GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                     wrap ? GL_REPEAT : GL_CLAMP );

    // build our texture mipmaps
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,
                       GL_RGB, GL_UNSIGNED_BYTE, data );

    // free buffer
    free( data );

    return texture;
}