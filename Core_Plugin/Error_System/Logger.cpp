#include "ErrorSystem.h"
using namespace std;


PluginData PLF_Init()
{
    PluginData ReturnInfo;
    ReturnInfo.Name = "pl0x Error System";
    ReturnInfo.Type = Error;
    return ReturnInfo;
}
bool PLF_Register(std::auto_ptr<Version_Info> Version_Data, PluginManager * handler)
{
    //simple version check.
    if(Version_Data->Major_Version < PLF_MAJOR_VERSION) return false;
    new ErrSys(handler);
    return true;
}

ErrSys::ErrSys(PluginManager * han) : ErrorServer (han, "pl0x Error Server")
{
    Handler->RegisterError(Name,std::auto_ptr<ErrorServer>(this));
    timeswritten = 0;
    LoggingStatus = true;
    ExecutionTime = GetTime(DATETIME);
    ERRFILE = NULL;
    KillFunc = NULL;
}

ErrSys::~ErrSys()
{
    if(ERRFILE!=NULL)
    {
        fclose(ERRFILE);
        ERRFILE = NULL;
    }
    if(KillFunc != NULL)
    {
        KillFunc = NULL;
    }
}

void ErrSys::Write(unsigned short ErrorLevel, std::string String, std::string Dump, std::string ExtendedData)
{	
	stringstream ss;
	ss << Dump << "] \n \t Extended Data:\t\t[" << ExtendedData ;
	Write(ErrorLevel,String, (ss.str().c_str()));
}

void ErrSys::Write(unsigned short ErrorLevel, std::string String, std::string Dump)
{
	/* Format of each error log.
		ERROR LEVEL:	[ErrorLevel]
			TIME:		[hh:mm:ss]
			ERROR:		[String]
			DUMP:		[Dump]
	*/

	if(ErrorLevel < 10) //Critical Error
	{
		Throw(String);
		LoggingStatus = true; //If critical, Enable logging
	}

	if(LoggingStatus) //Make sure logging is enabled
	{
		stringstream ss;
		ss << "\n\nERROR LEVEL:\t\t[" << ErrorLevel << "]\n\t TIME:\t\t[" << GetTime(TIME) 
			<< "]\n\t ERROR:\t\t[" << String << "]\n\t DUMP:\t\t[" << Dump  << ']';

		if (ERRFILE!=NULL) //Just make sure ERRFILE has been created before we write to it :]
		{
			fputs(ss.str().c_str(),ERRFILE);
			fflush(ERRFILE);
			timeswritten++; //LOL THIS MEANS I CAN SEE IF THE FILE IS EMPTY REAL SIMPLE LIKE LOLOLOL
		}
		else
		{
			ERRFILE = fopen(ExecutionTime.c_str(),"a+");
			if(ERRFILE!=NULL)
			{
				fputs(ss.str().c_str(),ERRFILE);
				fflush(ERRFILE);
				timeswritten++; //LOL THIS MEANS I CAN SEE IF THE FILE IS EMPTY REAL SIMPLE LIKE LOLOLOL
			}
		}
	}
}

bool ErrSys::ReadLast(char** String, unsigned int* Length)
{
	if (timeswritten <1)
	{
		Write(100,"No errors to load, Error call failed", "Error System Read-Last-Error");
		return false;
	};//Make sure the file has actually been written to.

	stringstream Error;
	int x;

	if (ERRFILE!=NULL)
	{
		while  (( x = fgetc(ERRFILE)) != EOF )
		{
			Error << x; // Puts the character into the string... 
		}
	}

	*Length = strlen(Error.str().c_str()); //Hopefully gets the length of the error

	return true;
}

string ErrSys::GetTime(unsigned short Type)
{
	char cDate[9], cTime[9];

        time_t mytime = time(NULL);
        strftime (cDate, 9, "%m/%d/%y", localtime(&mytime));
        strftime (cTime, 9, "%H:%M:%S", localtime(&mytime));
	stringstream ss;

	/*	
	Type is a simple seleciton so that this function can return either Date, Time, or Date 
	and Time together with an extension. 

	Date and Time Format: DD_MM_YY-hh_mm_ss <EXTENSION>
	Date format			: DD/MM/YY
	Time format			: hh:mm:ss
	*/

	switch (Type)
	{
	case 0:
		for (int i=0;i<9;i++)
		{
			//Change the non valid characters to underscores for use in filename
			if (cDate[i] == '\\' || cDate[i] == '/' || cDate[i] == ' ') cDate[i] = '_';
			if (cTime[i] == ':' || cTime[i] == ' ') cTime[i] = '_';
		}

		/* Add all the parts together, Extension is mainly for filetype extension
		 * (eg ".log") but can be anything else you want to add to the end
		 */
		ss << cDate << '-' << cTime <<".log\0";
		break;
	case 1:
		ss << cDate;
		break;
	case 2:
		ss << cTime;
		break;
	default:
		break;
	}

	string sDateTime = ss.str();

	return sDateTime;
}

void ErrSys::ChangeLogging(bool LoggerStatus)
{
	LoggingStatus = LoggerStatus;
}

bool ErrSys::GetLoggingStatus()
{
	return LoggingStatus;
}

void ErrSys::SetShutdownFunc(void (*ShutDownFunc)(bool))
{
	KillFunc = ShutDownFunc;
}

void ErrSys::Throw(std::string Error)
{
	Write(0,"Exception Enountered, Exiting", Error.c_str()); //Remeber to write the super massive error to the log.
	if(KillFunc){KillFunc(false);}			//if we have a killfunction we call it.
	throw std::exception();	//finally throw an exception to the OS
	return;
}