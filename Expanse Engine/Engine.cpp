/* 
 * File:   main.cpp
 * Author: pjb
 *
 * Created on 01 June 2011, 19:36
 */

#include <cstdlib>
#include "Engine.h"
#include <iostream>
#include <memory>
#include <list>
#include <ctime>
//windows entrypoint
#if defined _WIN32 || __CYGWIN__
	int main(int argc, char** argv);
	int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	{
		return main(0,NULL);
	}
#endif

double RND()
{
	double value = (rand() % 100);
	return (value / 50);
}

int main(int argc, char** argv) {
	 PluginFrameWork PLF;

#if defined _WIN32 || __CYGWIN__
	std::string mdlpath = ".\\SpacePlane.obj";
	MSG msg;
#else
	std::string mdlpath = "./SpacePlane.obj";
#endif
	std::string pluginPath = "./plugins/";

	//load plugins via new mechanic
	PLF.LoadPlugins(pluginPath);

	// Error system init
    ErrorServer * ERRSYS = PLF.GetPLF()->GrabError();
	if(!ERRSYS->init()) std::cout << "The error system Init failed";


	//Pre-init Scene &  Render grab (for physics)
	RenderServer* RenderTest = PLF.GetPLF()->GrabRender();
	SceneServer * SceneTest = PLF.GetPLF()->GrabScene();

	//Physics System Init
	PhysicsServer * PhysicsTest = PLF.GetPLF()->GrabPhysics();
	std::cout << "Starting Physics Init";
	PhysicsTest->RegisterScene(SceneTest);
	if(!PhysicsTest->init()) std::cout << "The Physics system Init failed";

	//OBJ Loader Init
    FileClass * OBJLoader = PLF.GetPLF()->GrabFile("OBJ");
	if(!OBJLoader->init()) std::cout << "The OBJ loader Init failed";
    std::cout << "       It Loads " << OBJLoader->GetExtention() << " files \n";

	//load Model
	std::cout << "Loading shotgun\n";
    unsigned int Shotgun = OBJLoader->Load(mdlpath);
    Base_Model * Shotgun_mdl = (Base_Model*)OBJLoader->Select(Shotgun);

	//Render System Init
    std::cout << "Starting Plugin Init\n";
	if(!RenderTest->init()) std::cout << "The Render system Init failed";

	//Scene System Init
	std::cout << "starting Scene Init";
	SceneTest->RegisterServers(RenderTest,PhysicsTest);
	if(!SceneTest->init()) std::cout << "The Scene system Init failed";


	//optimisation call for model
    std::cout << "Optimising shotgun\n";
	if(Shotgun_mdl != NULL) RenderTest->OptimiseModel(Shotgun_mdl);

	//Generate World Object
	World_Model_Object * TestObject = new World_Model_Object;
	if(Shotgun_mdl != NULL) TestObject->SetModel(Shotgun_mdl);
	// Note: Really REALLY REALLY need to add worldspace and global-space velocity options :/
	TestObject->SetVel(glm::vec3(0,0,-0.1));
	TestObject->SetRotVel(glm::vec3(0.0,0,0.5));
	glm::mat4 * scale =  TestObject->GetScale();
	*scale = glm::scale(*scale,glm::vec3(0.5,0.5,0.5));
	glm::mat4 * Translation = TestObject->GetTranslation();
	*Translation = glm::translate(*Translation, glm::vec3(0,0,-6));
	SceneTest->AddRenderObject(std::auto_ptr<World_Model_Object> (TestObject));
	std::list<unsigned int> Objects;
for(double z =9; z>=3; z-=3)
{
	for(double x =-5; x<=5; x+=2)
	{
		for(double y =-5; y<=5; y+=2)
		{
			if(!((x == 0) && (y == 0)))
			{
				TestObject = new World_Model_Object(TestObject);
				Translation = TestObject->GetTranslation();
				TestObject->SetRotVel(glm::vec3(0.0,0,1-RND()));
				*Translation = glm::translate(glm::mat4(), glm::vec3(x+RND(),y+RND(),z+RND()));
				TestObject->SetScale(glm::vec3(RND()/3,RND()/3,RND()/3));
				Objects.push_back(SceneTest->AddRenderObject(std::auto_ptr<World_Model_Object> (TestObject)));
			}
		}
	}
}


    std::cout << "Starting Render\n";
	bool done = false;
	clock_t Timer = clock();
	while(!done)									// Loop That Runs While done=FALSE
	{
			#if defined _WIN32 || __CYGWIN__
			if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
			{
				if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
				{
					done=true;							// If So done=TRUE
				}
				else									// If Not, Deal With Window Messages
				{
					TranslateMessage(&msg);				// Translate The Message
					DispatchMessage(&msg);				// Dispatch The Message
				}
			}
			#endif
			if(clock() > (Timer + 33))
			{
				for(std::list<unsigned int>::iterator IT = Objects.begin(); IT != Objects.end(); IT++)
				{
					World_Phys_Object * Current = SceneTest->GetPhysObject(*IT);
					//Current->SetScale(glm::vec3(RND()/3, RND()/3, RND()/3));
					//Current->SetScalar(glm::core::type::vec3(0.99, 0.99, 0.99));
				}

				Timer = clock();
				SceneTest->RunFrame();
			}
	}
	// Shutdown
	if(Shotgun_mdl != NULL)RenderTest->FreeModelData(Shotgun_mdl);
	RenderTest->CloseRender();
	return (0);							// Exit The Program
}