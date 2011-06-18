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
// $Id: acora.cpp 702 2009-01-31 18:50:56Z ankit $
//===================================================//

#include "instance.h"
#include "config.h"
#include "utils.h"
#include "exception.h"

#include <iostream>
#include <cerrno>
#include <cstring>
#include <csignal>
#include <cstdlib>

// services instance
Instance* instance;

namespace entry
{
	void quickExit(int)
	{
		exit(0);
	}
}

// program entry point
int main(int argc, char **argv)
{
	bool start = false, enableDebug = false;
	nstring::str arg;
	
	for (short i = 1; i < argc; ++i)
	{
		arg = argv[i];
		
		if (arg == "debug")
			start = enableDebug = true;
		else if (arg == "start")
			start = true;
		else if (arg == "version")
		{
			std::cout << "acora-" << config::VERSION << std::endl;
			return err::exit::normal;
		}
		else if (arg == "help")
		{
			std::cout << "\nUsage: acora [options]" << std::endl;
			std::cout << "The following options are supported:\n" << std::endl;
			std::cout << "start:    Start acora IRC Services normally" << std::endl;
			std::cout << "debug:    Start acora IRC Services in debug mode" << std::endl;
			std::cout << "version:  Display version and exit" << std::endl;
			std::cout << "help:     Display this message and exit\n" << std::endl;
			return err::exit::normal;
		}
	}
	
	if (!start)
	{
		std::cout << "Usage: acora [start|debug|version|help]" << std::endl;
		return err::exit::normal;
	}
	
	std::signal(SIGTERM, entry::quickExit);
	std::signal(SIGCHLD, entry::quickExit);
	
	// fork
	if (!enableDebug)
	{
		int i = fork();

		if (i > 0)
		{
			// wait for child to kill us, so the shell prompt
			// doesn't come back over output. if it threw an error
			// and died, we should die too
			while (kill(i, 0) != -1)
				sleep(1);

			return err::exit::normal;
		}
		else if (i < 0)
		{
			std::cout << "Could not fork(): " << std::strerror(errno) << std::endl;
			return err::exit::fork;
		}
		else if (setsid() == -1)
		{
			std::cout << "Could not setsid(): " << std::strerror(errno) << std::endl;
			return err::exit::setsid;
		}
	}
	
	std::signal(SIGTERM, SIG_DFL);
	std::signal(SIGCHLD, SIG_DFL);

	// return value	
	ErrorCode ret;
	
	// if any exception is not handled throughout the services,
	// they'll get caught here, so we put these two lines in a
	// try block and catch exceptions after it
	try
	{
		// initialize the instance
		instance = new Instance(argc, argv, enableDebug);
		// start the services
		ret = instance->start();
	}
	
	// catch an exception
	catch (Exception &e)
	{
		instance->log(ERROR, "main(): " + nstring::str() + "Exception thrown from " + e.getSource() + ": " + e.getReason());
		ret = err::exit::exception;
	}
	
	// catch a standard exception
	catch (std::exception &e)
	{
		instance->log(ERROR, "main(): " + nstring::str() + "Standard exception: " + e.what());
		ret = err::exit::exception;
	}
	
	instance->log(ERROR, "main(): Exit code: " + utils::toStr<ErrorCode>(ret));
	
	// finito
	delete instance;
	return ret;
}

