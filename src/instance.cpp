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
	void text(String s)
	{
		std::cout << "- " << s.c_str() << std::endl;
	}
	
	void end()
	{
		std::cout << "------------------------------" << std::endl;
	}
}


Instance::Instance(int &ac, char **av, const bool &d) :
	argc		(ac),
	argv		(av),
	enableDebug	(d),
	keepRunning	(true),
	now		(std::time(NULL)),
	moduleManager	(NULL),
	socketEngine	(NULL),
	ircdProtocol	(NULL),
	diskManager	(NULL),
	configReader	(NULL),
	userManager	(NULL),
	channelManager	(NULL)
{ }


#define DELETE(x) delete x; x = NULL;

void Instance::cleanup()
{
	debug("Cleaning up ...");

	// deleting moduleManager deletes socketEngine, ircdProtocol,
	// diskManager and loaded modules
	if (moduleManager)	DELETE(moduleManager);
	if (configReader)	DELETE(configReader);
	if (userManager)	DELETE(userManager);
	if (channelManager)	DELETE(channelManager);
	
	debug("Cleanup complete.");
}

#undef DELETE


Instance::~Instance()
{
	cleanup();
	debug("Shutting down ...");
}


ErrorCode Instance::start()
{
	banner::show();	

	// read config file
	configReader = new ConfigReader;
	
	std::deque<String> errors;
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
		
		for (std::deque<String>::iterator i = errors.begin(); i != errors.end(); ++i)
			banner::text("  - " + *i);
		
		if (r == err::configreader::errors)
		{
			banner::end();
			return err::exit::configreader;
		}
	}
	
	// create module manager
	moduleManager = new ModuleManager;

	// load socketengine
	String se;
	configReader->getValue(se, "modules", "socketengine");
	if (moduleManager->loadSocketEngine(se) != err::modulemanager::none)
	{
		banner::text("Could not load SocketEngine " + se + ": " + instance->error);
		banner::end();
		return err::exit::modulemanager;
	}

	// load ircdprotocol
	String ip;
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
		
		for (std::deque<String>::iterator i = errors.begin(); i != errors.end(); ++i)
			banner::text("  - " + *i);
	}
	
	if (!enableDebug && isatty(0) && isatty(1) && isatty(2) && kill(getppid(), SIGTERM) == -1)
	{
		banner::text(String() + "Error killing parent process: " + std::strerror(errno));
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
	String buffer;
	ErrorCode ret;

	while (keepRunning)
	{
		std::time(&now);
		ret = socketEngine->recv(buffer);
		
		if (ret == err::socketengine::none)
			ircdProtocol->processBuffer(buffer);
		else if (ret != err::socketengine::empty)
		{
			debug("recv returned error: " + error);
			break;
		}
	}

	return err::exit::normal;
}


std::time_t Instance::time(time_t since)
{
	return (now - since);
}


void Instance::exit(ErrorCode err)
{
	throw err;
}


void Instance::debug(String s)
{
	if (enableDebug)
		std::cout << "[DBG] " << s.c_str() << std::endl;
}


void Instance::finalize(String s)
{
	error = s;
	keepRunning = false;
}


bool Instance::isFinal()
{
	return !keepRunning;
}

