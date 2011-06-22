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
#include "modeparser.h"

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
	hasError(true),
	now(std::time(NULL)),
	moduleManager(NULL),
	socketEngine(NULL),
	ircdProtocol(NULL),
	diskManager(NULL),
	configReader(NULL),
	userManager(NULL),
	channelManager(NULL),
	modeParser(NULL)
{ }


#define DELETE(x) delete x; x = NULL;

void Instance::cleanup()
{
	log(DEBUG, "Instance(): Cleaning up ...");

	// deleting moduleManager deletes socketEngine, ircdProtocol,
	// diskManager and loaded modules
	if (moduleManager)	DELETE(moduleManager);
	if (configReader)	DELETE(configReader);
	if (userManager)	DELETE(userManager);
	if (channelManager)	DELETE(channelManager);
	if (modeParser)		DELETE(modeParser);
	
	log(DEBUG, "Instance(): Cleanup complete.");
}

#undef DELETE


Instance::~Instance()
{
	cleanup();
	log(DEBUG, "Instance(): Shutting down ...");
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
		showDebug();
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
			showDebug();
			return err::exit::configreader;
		}
	}
	
	// create module manager
	moduleManager = new ModuleManager;

	// load socketengine
	nstring::str se;
	configReader->getValue(se, "modules", "socketengine");
	if (moduleManager->loadSocketEngine(se) != err::modulemanager::none)
	{
		banner::text("Could not load SocketEngine " + se + ": " + instance->error);
		banner::end();
		showDebug();
		return err::exit::modulemanager;
	}

	// load ircdprotocol
	nstring::str ip;
	configReader->getValue(ip, "modules", "ircdprotocol");
	if (moduleManager->loadIRCdProtocol(ip) != err::modulemanager::none)
	{
		banner::text("Could not load IRCdProtocol " + ip + ": " + instance->error);
		banner::end();
		showDebug();
		return err::exit::ircdprotocol;
	}
	
	// connect to remote server
	if (socketEngine->connect() != err::socketengine::none)
	{
		banner::text("Could not connect: " + instance->error);
		banner::end();
		showDebug();
		return err::exit::socketengine;
	}
	
	// load autoload modules
	moduleManager->loadAutoloadModules(errors);
	
	if (!errors.empty())
	{
		banner::text("The following modules could not be loaded:");
		
		for (std::deque<nstring::str>::iterator i = errors.begin(); i != errors.end(); ++i)
			banner::text("  - " + *i);
		banner::end();
		showDebug();
	}
	
	if (!enableDebug && isatty(0) && isatty(1) && isatty(2) && kill(getppid(), SIGTERM) == -1)
	{
		banner::text(nstring::str() + "Error killing parent process: " + std::strerror(errno));
		banner::end();
		showDebug();
	}

	if (!enableDebug)
	{
		for (short i = 0; i <= 2; ++i)
		{
			if (isatty(i))
				close(i);
		}
	}
	
	showDebug();
	
	// initialize user and channel managers
	userManager = new UserManager;
	channelManager = new ChannelManager;
	modeParser = new ModeParser;

	return run();
}

/**
 Instance::run

 main loop
 */
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
			log(ERROR, "Instance(): recv returned error: " + error);
			break;
		}
	}

	return err::exit::normal;
}


/**
 Instance::onRehash

 function called upon loading of config file
 */
void Instance::onRehash()
{
	// set our log level (std::vector<int>) (believe it or not)
    nstring::str loglevel = "";
    std::vector<nstring::str> loglevels;

    logLevel.clear();
    configReader->getValue(loglevel, "settings", "loglevel");

    // convert our log level into numerics
    utils::explode(" ", loglevel, loglevels);
    for (std::vector<nstring::str>::iterator i = loglevels.begin(); i != loglevels.end(); ++i)
    {
        if (*i == "all")
            logLevel.push_back(ALL);
        if (*i == "debug")
            logLevel.push_back(DEBUG);
		if (*i == "misc")
            logLevel.push_back(MISC);
        if (*i == "error")
            logLevel.push_back(ERROR);
        if (*i == "network")
            logLevel.push_back(NETWORK);
        if (*i == "commands")
            logLevel.push_back(COMMANDS);
        if (*i == "admin")
            logLevel.push_back(ADMIN);
        if (*i == "register")
            logLevel.push_back(REGISTER);
		if (*i == "set")
			logLevel.push_back(SET);
        if (*i == "rawdata")
            logLevel.push_back(RAWDATA);
    }
	//logIterator = logLevel.find(ALL);
	std::sort(loglevel.begin(), loglevel.end());
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
	std::stringstream output;
	output << "[" << formattedTime << "] " << textBuffer;
	
	if (std::binary_search(logLevel.begin(), logLevel.end(), 0) || std::binary_search(logLevel.begin(), logLevel.end(), type))
    {
		if (enableDebug && !hasError)
			std::cout << output.str().c_str() << std::endl;
		else if (enableDebug && hasError)
			debugBuffer.push_back(output.str().c_str());
		// std cout it if enableDebug is on
		
		// log file it.
	}

    va_end(argsPtr);
}

void Instance::showDebug()
{
	hasError = false;
	if (enableDebug && !debugBuffer.empty())
	{
		std::vector<nstring::str>::iterator it;
		for (it = debugBuffer.begin(); it != debugBuffer.end(); ++it)
			std::cout << (*it).c_str() << std::endl;
		debugBuffer.clear();
	}
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

