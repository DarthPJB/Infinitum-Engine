#ifndef ERROR_SYSTEM_PLUGIN
#define ERROR_SYSTEM_PLUGIN

#include <ctime>	// Used for time functions
#include <fstream>	// Use for file functions
#include <string>       // Needed to manipulate strings for filenames
#include <cstring>      //gnu does not include this with string
#include <sstream>	// Same as above
#include <exception>
#include "Plugin_Static.h"      //static plugin ninja-ness

class ErrSys : public ErrorServer
{
public:
	enum time {DATETIME,DATE,TIME};
        ErrSys(PluginManager* han);
        ~ErrSys();
        virtual bool PreInit() {return true;}
        virtual bool init(){return true;}
        virtual void Write(unsigned short ErrorLevel, std::string String, std::string Dump);
        virtual void Write(unsigned short ErrorLevel, std::string String, std::string Dump, std::string ExtendedData);
        virtual bool ReadLast(char** String, unsigned int* Length);
        virtual void ChangeLogging(bool LoggerStatus);	//Used to Enable or Disable ErrSys.
        virtual bool GetLoggingStatus();				//Used to retreive the current status of the logger (Enabled/Disabled)
        virtual void SetShutdownFunc(void (*ShutDownFunc)(bool));
        virtual void Throw(std::string);
private:
        void (*KillFunc)(bool);
        std::string GetTime(unsigned short Type); //Used to get exact time.
        FILE *ERRFILE;				//Pointer to the log file.
        std::string ExecutionTime;	//Holds exact time of initial execution
        int timeswritten;			//Number of times the log file has been written to
        bool LoggingStatus;			//Status of logging (Enabled/Disabled)
};

#endif