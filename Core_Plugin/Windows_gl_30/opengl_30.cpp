#include <vector>

#include "opengl_30.h"
#include "soil/SOIL.h"
#include <fstream>
#include <sstream>

PluginData PLF_Init()
{
    PluginData RetVal;
    RetVal.Name = "Windows_GL_30";
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
	//plugin will self register and engine should handle memory cleanup.
    new Render_Windows_GL_30(handler);
    return true;
}
Render_Windows_GL_30::Render_Windows_GL_30(PluginManager* han) : RenderServer(han, "Windows_GL_30")
{
	Handler->RegisterRender(Name,std::auto_ptr<RenderServer> (this));
    Frame=0;
}
Render_Windows_GL_30::~Render_Windows_GL_30()
{
	if(Basic != NULL) delete Basic;
    CloseRender();
}

bool Render_Windows_GL_30::PreInit()
{
    ERRSYS = Handler->GrabError();
    return true;                //actual checking is needed here, but for now we 'fake it'
}
bool Render_Windows_GL_30::init()
{
	if(ERRSYS==NULL)ERRSYS = Handler->GrabError();
	//hacking some shit down
	int width = 1024;
	int height = 768;
	RECT rc;
	GetWindowRect(GetDesktopWindow(), &rc);
	width = rc.right;
	height = rc.bottom;
	bool fullscreenflag = false;
	int bits = 32;
	char * title = "Windows OpenGL Expanse";

	Normal=-1;
	Vertex=-1;
	Colour=-1;
	UV=-1;
	Edge=-1;
	Projection=-1;
	Transform=-1;

 	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "ExpanseEngine";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"ExpanseEngine",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		CloseRender();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support float Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		CloseRender();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		CloseRender();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		CloseRender();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	HGLRC tempContext;
	if (!(tempContext=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		CloseRender();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	if(!wglMakeCurrent(hDC,tempContext))					// Try To Activate The Rendering Context
	{
		CloseRender();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 0,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0
	};

	PFNWGLCREATECONTEXTATTRIBSARBPROC windowsglCreateContextAttribsARB = NULL; //pointer to the 
	windowsglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");
	if(wglCreateContextAttribsARB != NULL)
	{
		hRC = windowsglCreateContextAttribsARB(hDC,0, attribs);
		wglMakeCurrent(NULL,NULL);
		wglDeleteContext(tempContext);
		wglMakeCurrent(hDC, hRC);
	}
	else
	{	//It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
		//@todo Error
		hRC = tempContext;
	}

 	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		MessageBox(NULL,"GLEW is not initialized!","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	UpdateWindow(hWnd);
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReScaleView(width, height, true);					// Set Up Our Perspective GL Screen
	
	//init
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glClearColor(0.1f, 0.1f, 0.1f, 0.5f);				// Black Background

	//The following is for testing purposes only (due to texture and shader support not being fully implimented engine wide)
	Basic = new ShaderManager(ERRSYS);
	unsigned int vert = Basic->LoadGlobalShader("minimal.vert", true);
	unsigned int frag = Basic->LoadGlobalShader("minimal.frag", false);

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
	//REMINDER: Check is this flag is still a prerequisite when using shaders for texturing
	glEnable(GL_TEXTURE_2D);

	//generate simple shader and assign attributes
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

	//bind textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texture_val,0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniform1i(texture_val2,1);


	if(!hRC) return false;
	return TRUE;									// Success
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

		case WM_SIZE:								// Resize The OpenGL Window
		{
			//Need to re-impliment this including GL-data reloading for windows (as windows will
			//clear local GPU data should this event occour
			return 0;								// Jump Back
		}
	}

	// Pass All Unhandled Messages To Default Window Proc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

void Render_Windows_GL_30::CloseRender()
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("ExpanseEngine",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
    return;
}
void Render_Windows_GL_30::RenderModel(Base_Model* Model)
{
    if(Model != NULL)
    {
        for(std::vector<Base_Mesh*>::iterator Cur_Mesh=Model->Meshes.begin(); Cur_Mesh < Model->Meshes.end(); Cur_Mesh++)
        {
            RenderMesh(*Cur_Mesh);
        }
    }
}
void Render_Windows_GL_30::RenderModel(Base_Model* Model, glm::mat4 Matrix)
{
	//backup any existing matrix
	MODELVIEW_MAT_1 = MODELVIEW_MAT;
	//impliment transformations using passed matrix from scene manager
	MODELVIEW_MAT *= Matrix;
    if(Model != NULL)
    {
        for(std::vector<Base_Mesh*>::iterator Cur_Mesh=Model->Meshes.begin(); Cur_Mesh < Model->Meshes.end(); Cur_Mesh++)
        {
            RenderMesh(*Cur_Mesh);
        }
    }
	MODELVIEW_MAT = MODELVIEW_MAT_1;
}
void Render_Windows_GL_30::RenderMesh(Base_Mesh* Mesh)
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
	//pass transformation and projection matrix to shader (REMINDER: this needs to be implimented as a global for world objects)

	glUniformMatrix4fv(Projection, 1, GL_FALSE, &PROJECTION_MAT[0][0]);
	glUniformMatrix4fv(Transform, 1, GL_FALSE, &MODELVIEW_MAT[0][0]);
	glm::mat4 InverseTranspose = MODELVIEW_MAT * PROJECTION_MAT;
	InverseTranspose = glm::inverse(glm::transpose(InverseTranspose));
	glUniformMatrix4fv(InvTrans, 1, GL_FALSE, &InverseTranspose[0][0]);

    if(Mesh->SpecialUsed) 
	{
        if(Mesh->SpecialIndex1 != 0)
        {       //Vertex Buffer Object mode
            
            if(Mesh->SpecialIndex2 != 0)
            {   // Vertex Buffer Array mode
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
				ERRSYS->Write(50,"Error, unexpected model format passed to renderer, continuing without render:", "ERROR");
				return;
            }
        }
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        return;
    }
	//if we have reached here there is no optimisation on this model (phys-model in render mode?) manually index and render.
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
    
	//begin render mode tests
    if(VertexUsed || InterleavedUsed)
    {        //You can't draw without any vertex data
        if(InterleavedUsed && (!VertexUsed))
        {        //You can't draw without any vertex data
            if(!InterLeaved->CurrentlyUsed('v'))return;
        }
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
			//Drawing non-indexed list (Plough though in order)
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
			//index list avalible, render by element id.
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

void Render_Windows_GL_30::reshape2D(GLint w, GLint h)
{
	PROJECTION_MAT = glm::ortho(0.0f, (float)w, 0.0f, (float)h);
    return;
}
void Render_Windows_GL_30::reshape3D(GLint w, GLint h)
{
	PROJECTION_MAT = glm::frustum(-1.0,1.0,-1.0,1.0,1.0,100.0);
    return;
}
void Render_Windows_GL_30::ReScaleView(int w, int h,bool threeD)
{
	glEnable(GL_DEPTH_TEST);
    glViewport(0,0,(GLsizei)w,(GLsizei)h);
    if(threeD) reshape3D((GLint)w,(GLint)h);
    else reshape2D((GLint)w,(GLint)h);
}

void Render_Windows_GL_30::ReScaleWindow(int w, int h)
{
    return;            //<<<<<<< REMINDER: This needs implimenting
}
bool Render_Windows_GL_30::ToggleFullScreen(bool)
{
    return false;       //<<<<<<< REMINDER:  This needs implimenting
}

void Render_Windows_GL_30::FrameEnd()
{
    SwapBuffers(hDC);           // Swap Buffers (float Buffering)
    return;
}
void Render_Windows_GL_30::FrameStart()
{
    Frame++;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	MODELVIEW_MAT = glm::mat4();
    return;
}
void Render_Windows_GL_30::OptimiseModel(Base_Model * Model)
{
    //call model opt
    Model->Optimise();  //also calls mesh opt, not needed later
	if(OpenGLVersion[0] >=3) 
	{
		for(std::vector<Base_Mesh*>::iterator touch_my_little_tinkle = Model->Meshes.begin(); touch_my_little_tinkle < Model->Meshes.end(); touch_my_little_tinkle++)
		{
			if((*touch_my_little_tinkle)!=NULL)OptimiseMesh((*touch_my_little_tinkle));
		}
	}
    //could do whole-model opt here (if we wanted)
 return;   
}
void Render_Windows_GL_30::FreeModelData(Base_Model * Model)
{
    for(std::vector<Base_Mesh*>::iterator touch_my_little_tinkle = Model->Meshes.begin(); touch_my_little_tinkle < Model->Meshes.end(); touch_my_little_tinkle++)
    {
        if((*touch_my_little_tinkle)!=NULL)FreeMeshData((*touch_my_little_tinkle));
    }    
}
void Render_Windows_GL_30::FreeMeshData(Base_Mesh * Mesh)
{
    if(Mesh->SpecialIndex3 > 0)
    {
        glDeleteBuffers(Mesh->SpecialIndex3,(GLuint*)Mesh->SpecialIndex1);
        delete [] (GLuint*)Mesh->SpecialIndex1;
        glDeleteVertexArrays(1,(GLuint*)&Mesh->SpecialIndex2);
    }
}
void Render_Windows_GL_30::OptimiseMesh(Base_Mesh * Mesh)
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
    return;
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
int Render_Windows_GL_30::isExtensionSupported(const char *extension)
{
  const GLubyte *extensions = NULL;
  const GLubyte *start;
  GLubyte *where, *terminator;

  /* Extension names should not have spaces. */
  where = (GLubyte *) strchr(extension, ' ');
  if (where || *extension == '\0')
    return 0;

  extensions = glGetString(GL_EXTENSIONS);

  /* It takes a bit of care to be fool-proof about parsing the
     OpenGL extensions string.  Don't be fooled by sub-strings,
     etc. */
  start = extensions;
  for (;;) {
    where = (GLubyte *) strstr((const char *) start, extension);
    if (!where)
      break;
    terminator = where + strlen(extension);
    if (where == start || *(where - 1) == ' ')
      if (*terminator == ' ' || *terminator == '\0')
        return 1;
    start = terminator;
  }
  return 0;
}