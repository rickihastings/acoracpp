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
// $Id: ircdprotocol.h 708 2009-02-01 11:48:00Z ankit $
//===================================================//

#ifndef	IRCDPROTOCOL_H
#define	IRCDPROTOCOL_H

#include "extensible.h"

#include <map>

// ircdprotocol errors
namespace err
{
	namespace ircdprotocol
	{
		enum
		{
			none,
			alreadyExists
		};
	}
}

class IRCdCommand;
class Server;

// base class for IRCd protocols, these modules provide
// a way of communicating with the remote server
// depending on its version
class IRCdProtocol : public Base
{
public:
	// module handle
	const void* handle;
	// module version
	const String version;

	// ircd requires a numeric to be specified
	bool requireNumeric;
	
	// some variables that tell the state of the link, ie during burst, finished burst. etc
	bool finishedBurst;
	bool duringBurst;

	// commands that trigger events
	std::map<String, IRCdCommand*> commands;
	// servers
	std::map<String, Server*> servers;

	// constructor
	IRCdProtocol(void*, const String version);
	// destructor
	virtual ~IRCdProtocol();

	// process buffer
	virtual void processBuffer(String&, IRCdCommand* parent = NULL, String src = String());
	// add a command
	virtual ErrorCode addCommand(IRCdCommand*);
};

// macro to initialize an ircd protocol
#define initIRCdProtocol(class_name) \
	extern "C" IRCdProtocol* getIRCdProtocol(void* handle) \
	{ \
		return new class_name(handle); \
	}

#endif // IRCDPROTOCOL_H

