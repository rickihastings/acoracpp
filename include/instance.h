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

#include "base.h"

/**
 debug variables
 */
#define ALL 0
#define INFO 1

#define ERROR 2
#define NETWORK 3
#define COMMANDS 4
#define ADMIN 5
#define REGISTER 6
#define RAWDATA 7
#define LOGCHAN 8

/**
 max buffer - irc data
 */
#define MAXBUF 512

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
class ModeParser;
class SocketEngine;
class IRCdProtocol;
class DiskManager;

// an instance of the services
class Instance
{
	// arguments
	int argc;
	char **argv;

	// enable debug
	const bool enableDebug;
	// services keep running till this value is true
	bool keepRunning;
	// loglevel
	std::map<int, nstring::str> logLevel;
	// log level iterators
	std::map<int, std::map<int, nstring::str>::iterator> iterators;

public:

	// time
	std::time_t now;

	// last error
	nstring::str error;

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
	// mode parser
	ModeParser* modeParser;

	// constructor
	Instance(int &argc, char **argv, const bool &enabledebug);
	// destructor
	~Instance();

	// get time
	std::time_t time(time_t since = 0);

	// start the services
	ErrorCode start();
	// run the iterations
	ErrorCode run();
	
	// log
	void log(int type, const nstring::str &text, ...);
	
	// clean up everything and prepare to die
	void cleanup();
	// exit improperly
	void exit(ErrorCode);

	// make this iteration the final one, exit the services when this is over
	// error is set to the string passed
	void finalize(nstring::str = nstring::str());
	// true if this is the final iteration
	bool isFinal();
};

// services instance
extern Instance* instance;

#endif // INSTANCE_H

