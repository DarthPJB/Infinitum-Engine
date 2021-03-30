#include "Example Plugin.h"
/* Step three: Fill out the REQUIREd return functions (defined in plugin-static.h):
 * 
 *      PluginData PLF_Init();  
 *              the initial call function, is expected to fill out the structure
 *      bool PLF_Register(std::auto_ptr<Version_Info>, PluginManager *)
 *              The registration function, this *can* call the managers register
 *              function, however self-registration is recommended (see step four)
 * */

PluginData PLF_Init()
{
    PluginData RetVal;
    RetVal.Name = "PlugName";   /*this name is that of the entire plugin
                                 * Note: if only one module is in this plugin you
                                 * may simply apply the same name for this and the
                                 * module. However if more than one module is held
                                 * by this plugin you need to fill out the: */
    RetVal.Modules.clear();     //Module names vector, containing the name of 
                                //every module included in this plugin (exception above)
    
    RetVal.Type = Render;         /* the type of plugin this is (doesn't do much) */
    RetVal.Version.Major_Version = PLF_MAJOR_VERSION;
    RetVal.Version.Minor_Version = PLF_MINOR_VERSION;
    RetVal.Version.Func_Version = PLF_FUNC_VERSION;
    return RetVal;
}
bool PLF_Register(std::auto_ptr<Version_Info> Version_Data, PluginManager * handler)
{
    //Some kind of version check would be placed here, in this example it's simple
    if(Version_Data->Major_Version < PLF_MAJOR_VERSION) return false;
    //next we create the module instances and register them. 
    //For this example the registration can be seen in step four (self registration)
    //though there is no reason you could not manually register them here.
    //eg: Handler->RegisterRender("Object Name",std::auto_ptr<RenderServer>(Pointer_To_Object));
    new Render_LINUX_GL_30(handler);
    return true;
}

/* 
 * Step four, pass the handler and name info to the base-constructor
 *      (then RECCOMENDED, add the registration call)
 * 
 */
ErrSys::ErrSys(PluginManager* han) : ErrorServer(han, "PlugName")
{
    Handler->RegisterError(Name,std::auto_ptr<ErrorServer>(this));
    //using the this pointer with the auto_ptr means memory will be auto-cleaned.
    // no need for any garbage collection ^_^;;
    
};
ErrSys::~ErrSys()
{
    //standard destructor
}

/* 
 * Step five:
 * Add the init and preinit routines, 
 * 
 *      preinit should do the minimum possible checks to ensure the module works
 *      on the system
 * 
 *      init should initialise the module, assign the error system (if avalible)
 *      and report false for an error.
 * 
 */
bool ErrSys::PreInit()
{
    return true;  
};
bool ErrSys::init()
{
    ERRSYS = Handler->GrabError();
    return true;  
};
/* 
 * Step six:
 * Get stuff done, make the code for the module:
 */
void ErrSys::Write(unsigned short ErrorLevel, std::string String, std::string Dump)
{
    
}
void ErrSys::Write(unsigned short ErrorLevel, std::string String, std::string Dump, std::string ExtendedData)
{
    
}
bool ErrSys::ReadLast(char** String, unsigned int* Length)
{
 return true;   
}
void ErrSys::ChangeLogging(bool LoggerStatus)
{
    
}
bool ErrSys::GetLoggingStatus()
{
 return true;   
}
void ErrSys::SetShutdownFunc(void (*ShutDownFunc)(bool))
{
    
}
void ErrSys::Throw(std::string)
{
    
}