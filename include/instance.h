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
// $Id: instance.h 706 2009-01-31 20:17:11Z ankit $
//===================================================//

#ifndef	INSTANCE_H
#define	INSTANCE_H

#include "extensible.h"

#include <ctime>

// exit codes
namespace err
{
	namespace exit
	{
		enum
		{
			normal,
			fork,
			setsid,
			configreader,
			socketengine,
			ircdprotocol,
			modulemanager,
			exception
		};
	}
}

class ModuleManager;
class ConfigReader;
class UserManager;
class ChannelManager;
class SocketEngine;
class IRCdProtocol;
class DiskManager;

// an instance of the services
class Instance : public Extensible
{
	// arguments
	int argc;
	char **argv;

	// enable debug
	const bool enableDebug;
	// services keep running till this value is true
	bool keepRunning;

	// time
	std::time_t now;
public:

	// last error
	String error;

	// module manager
	ModuleManager* moduleManager;

	// socket engine
	SocketEngine* socketEngine;
	// ircd protocol
	IRCdProtocol* ircdProtocol;
	// disk manager
	DiskManager* diskManager;

	// configuration file
	ConfigReader* configReader;
	
	// user manager
	UserManager* userManager;
	// channel manager
	ChannelManager* channelManager;

	// constructor
	Instance(int &argc, char **argv, const bool &enabledebug);
	// destructor
	~Instance();

	// get time
	std::time_t time(time_t since = 0);
	// debug message
	void debug(const String);

	// start the services
	ErrorCode start();
	// run the iterations
	ErrorCode run();

	// clean up everything and prepare to die
	void cleanup();
	// exit improperly
	void exit(ErrorCode);

	// make this iteration the final one, exit the services when this is over
	// error is set to the string passed
	void finalize(String = String());
	// true if this is the final iteration
	bool isFinal();
};

// services instance
extern Instance* instance;

#endif // INSTANCE_H

