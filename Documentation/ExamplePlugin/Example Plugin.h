/* 
 * File:   Example Plugin.h
 * Author: pjb
 *
 * Created on 23 September 2011, 22:26
 */

#ifndef EXAMPLE_PLUGIN_H
#define	EXAMPLE_PLUGIN_H

#include "Plugin_Static.h"
/* This file is an example header showing how to impliment a plugin for the Expanse engine
 * 
 * Step one: Define one (or more) plugin classes inheriting the base class, in this example we use the error system */

class ErrSys : public ErrorServer
{
public:
    /*
     *  Step two: Define the functions required by the base class
     *          (The base classes and thier required functions can be found in
     *          the shared-header file: 'plugin_header.h')
     * 
     * Note, the constructor is a requirement (you'll see why soon)
     */
    ErrSys(PluginManager* han);
    ~ErrSys();
    virtual bool init();
    virtual bool PreInit();
    virtual void Write(unsigned short ErrorLevel, std::string String, std::string Dump);
    virtual void Write(unsigned short ErrorLevel, std::string String, std::string Dump, std::string ExtendedData);
    virtual bool ReadLast(char** String, unsigned int* Length);
    virtual void ChangeLogging(bool LoggerStatus);
    virtual bool GetLoggingStatus();
    virtual void SetShutdownFunc(void (*ShutDownFunc)(bool));
    virtual void Throw(std::string);
private:
    /*
     * You can add additonal functionality as long as you meet the demands of the existing system
     * and don't REQUIRE that functionallity be met for the plugin to work.
     * 
     * (though a plugin can require another plugin to function if needs be)
     * 
     * Contiune to CPP file
     */
};

#endif	/* EXAMPLE_PLUGIN_H */

