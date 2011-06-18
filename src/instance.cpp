//===================================================//
//                                                   //
//              ~ Acora IRC Services ~               //
//                                                   //
//       Acora (C) 2011 Acora Development Team       //
// Please see the file CREDITS for more information. //
//                                                   //
//   This program is free but copyrighted software   //
//      Please see the file COPYING for details.     //
//                                                   //
//===================================================//
// $Id: instance.cpp 708 2009-02-01 11:48:00Z ankit $
//===================================================//

#include "instance.h"
#include "modulemanager.h"
#include "configreader.h"
#include "socketengine.h"
#include "ircdprotocol.h"
#include "utils.h"
#include "config.h"
#include "usermanager.h"
#include "channelmanager.h"

#include <iostream>
#include <cerrno>
#include <cstring>
#include <csignal>
#include <vector>

// functions used by run() for banner
namespace banner
{
	// display main banner
	void show()
	{
		std::cout << "Starting acora irc services..." << std::endl;
		std::cout << "------------------------------" << std::endl;
	}
	
	// display text after banner
	void text(nstring::str s)
	{
		std::cout << "- " << s.c_str() << std::endl;
	}
	
	void end()
	{
		std::cout << "------------------------------" << std::endl;
	}
}


Instance::Instance(int &ac, char **av, const bool &d) :
	argc(ac),
	argv(av),
	enableDebug(d),
	keepRunning(true),
	now(std::time(NULL)),
	moduleManager(NULL),
	socketEngine(NULL),
	ircdProtocol(NULL),
	diskManager(NULL),
	configReader(NULL),
	userManager(NULL),
	channelManager(NULL)
{ }


#define DELETE(x) delete x; x = NULL;

void Instance::cleanup()
{
	log(INFO, "Instance(): Cleaning up ...");

	// deleting moduleManager deletes socketEngine, ircdProtocol,
	// diskManager and loaded modules
	if (moduleManager)	DELETE(moduleManager);
	if (configReader)	DELETE(configReader);
	if (userManager)	DELETE(userManager);
	if (channelManager)	DELETE(channelManager);
	
	log(INFO, "Instance(): Cleanup complete.");
}

#undef DELETE


Instance::~Instance()
{
	cleanup();
	log(INFO, "Instance(): Shutting down ...");
}


ErrorCode Instance::start()
{
	banner::show();	

	// read config file
	configReader = new ConfigReader;
	
	std::deque<nstring::str> errors;
	ErrorCode r = configReader->read("acora.conf", errors);
	
	if (r == err::configreader::notFound)
	{
		banner::text("Could not open config file \"acora.conf\"");
		banner::end();
		return err::exit::configreader;
	}
	else if (!errors.empty())
	{
		if (r == err::configreader::errors)
			banner::text("There were errors in your config file:");
		else
			banner::text("There were warnings in your config file:");
		
		for (std::deque<nstring::str>::iterator i = errors.begin(); i != errors.end(); ++i)
			banner::text("  - " + *i);
		
		if (r == err::configreader::errors)
		{
			banner::end();
			return err::exit::configreader;
		}
	}
	
	// set our log level (std::vector<int>) (believe it or not)
    /*nstring::str loglevel = "info error network rawdata logchan";
    std::vector<nstring::str> loglevels;
	
	utils::explode(" ", loglevel, loglevels);
    for (std::vector<nstring::str>::iterator i = loglevels.begin(); i != loglevels.end(); ++i)
    {
        if (*i == "all")
            logLevel[ALL] = *i;
        if (*i == "info")
            logLevel[INFO] = *i;
        if (*i == "error")
            logLevel[ERROR] = *i;
        if (*i == "network")
            logLevel[NETWORK] = *i;
        if (*i == "commands")
            logLevel[COMMANDS] = *i;
        if (*i == "admin")
            logLevel[ADMIN] = *i;
        if (*i == "register")
            logLevel[REGISTER] = *i;
        if (*i == "rawdata")
            logLevel[RAWDATA] = *i;
        if (*i == "logchan")
            logLevel[LOGCHAN] = *i;
    }
    iterators[ALL] = logLevel.find(ALL);
    iterators[INFO] = logLevel.find(INFO);
    iterators[RAWDATA]= logLevel.find(RAWDATA);*/
	
	// create module manager
	moduleManager = new ModuleManager;

	// load socketengine
	nstring::str se;
	configReader->getValue(se, "modules", "socketengine");
	if (moduleManager->loadSocketEngine(se) != err::modulemanager::none)
	{
		banner::text("Could not load SocketEngine " + se + ": " + instance->error);
		banner::end();
		return err::exit::modulemanager;
	}

	// load ircdprotocol
	nstring::str ip;
	configReader->getValue(ip, "modules", "ircdprotocol");
	if (moduleManager->loadIRCdProtocol(ip) != err::modulemanager::none)
	{
		banner::text("Could not load IRCdProtocol " + ip + ": " + instance->error);
		banner::end();
		return err::exit::ircdprotocol;
	}
	
	// connect to remote server
	if (socketEngine->connect() != err::socketengine::none)
	{
		banner::text("Could not connect: " + instance->error);
		banner::end();
		return err::exit::socketengine;
	}
	
	// load autoload modules
	moduleManager->loadAutoloadModules(errors);
	
	if (!errors.empty())
	{
		banner::text("The following modules could not be loaded:");
		
		for (std::deque<nstring::str>::iterator i = errors.begin(); i != errors.end(); ++i)
			banner::text("  - " + *i);
	}
	
	if (!enableDebug && isatty(0) && isatty(1) && isatty(2) && kill(getppid(), SIGTERM) == -1)
	{
		banner::text(nstring::str() + "Error killing parent process: " + std::strerror(errno));
		banner::end();
	}

	if (!enableDebug)
	{
		for (short i = 0; i <= 2; ++i)
		{
			if (isatty(i))
				close(i);
		}
	}
	
	// initialize user and channel managers
	userManager = new UserManager;
	channelManager = new ChannelManager;

	return run();
}


ErrorCode Instance::run()
{
	nstring::str buffer;
	ErrorCode ret;

	while (keepRunning)
	{
		std::time(&now);
		ret = socketEngine->recv(buffer);
		
		if (ret == err::socketengine::none)
			ircdProtocol->processBuffer(buffer);
		else if (ret != err::socketengine::empty)
		{
			log(NETWORK, "Instance(): recv returned error: " + error);
			break;
		}
	}

	return err::exit::normal;
}

/**
 Instance::debug

 debug
 */
void Instance::log(int type, const nstring::str &text, ...)
{
    // variables
    char formattedTime[9];
    struct tm * timeInfo = localtime(&now);

    // generate our time
    strftime(formattedTime, sizeof(formattedTime), "%X", timeInfo);

    char textBuffer[MAXBUF+1] = "";
    va_list argsPtr;

    va_start(argsPtr, text);
    vsprintf(textBuffer, text.c_str(), argsPtr);

    std::map<int, nstring::str>::iterator i = logLevel.find(type);

    if (enableDebug)
		std::cout << "[" << formattedTime << "] " << textBuffer << std::endl;
	
	// logchan etc.

    va_end(argsPtr);
}

std::time_t Instance::time(time_t since)
{
	return (now - since);
}


void Instance::exit(ErrorCode err)
{
	throw err;
}


void Instance::finalize(nstring::str s)
{
	error = s;
	keepRunning = false;
}


bool Instance::isFinal()
{
	return !keepRunning;
}

