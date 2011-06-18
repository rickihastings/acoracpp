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

#ifndef	IRCDPROTOCOL_H
#define	IRCDPROTOCOL_H

#include "base.h"

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
	const nstring::str version;

	// ircd requires a numeric to be specified, some more setting based variables
	// data from the ircd
	bool requireNumeric;
	int maxParams;
	nstring::str capabModes;
	nstring::str prefixData;
	nstring::str defaultChanModes;
	
	// post parsed data from the ircd
	nstring::str modes;
	nstring::str restrictModes;
	nstring::str statusModes;
	nstring::str modesWithParams;
	nstring::str modesWithUParams;
	std::map<char, char> prefixModes;
	bool owner;
	bool protect;
	bool halfop;
	
	// some variables that tell the state of the link, ie during burst, finished burst. etc
	bool finishedBurst;
	bool duringBurst;

	// commands that trigger events
	std::map<nstring::str, IRCdCommand*> commands;
	// servers
	std::map<nstring::str, Server*> servers;

	// constructor
	IRCdProtocol(void*, const nstring::str version);
	// destructor
	virtual ~IRCdProtocol();

	// process buffer
	virtual void processBuffer(nstring::str&, IRCdCommand* parent = NULL, nstring::str src = nstring::str());
	// add a command
	virtual ErrorCode addCommand(IRCdCommand*);
	
	// add server
	void addServer(nstring::str&, nstring::str&);
	// remove server
	void remServer(nstring::str&);

	// parse modes from capab
	void parseModes(nstring::str&);
};

// macro to initialize an ircd protocol
#define initIRCdProtocol(class_name) \
	extern "C" IRCdProtocol* getIRCdProtocol(void* handle) \
	{ \
		return new class_name(handle); \
	}

#endif // IRCDPROTOCOL_H

