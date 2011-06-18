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
// $Id: ircd_charybdis.h 702 2009-01-31 18:50:56Z ankit $
//===================================================//

#include "ircdprotocol.h"
#include "ircdcommand.h"
#include "server.h"

#include <deque>

// a charybdis server
class charybdisServer
{
public:
	// constructor
	charybdisServer();
	// destructor
	~charybdisServer();
};

// the charybdis ircd protocol class
class charybdisProtocol : public IRCdProtocol
{
public:
	// loaded modules
	std::deque<std::string> modules;
	// capabilities
	std::map<std::string, nstring::str> capabs;
	// our SID
	nstring::str sid;
	// our server name
	nstring::str name;
	
	nstring::str linkSid;
	// their server id
	nstring::str linkName;
	// their server name

	// constructor
	charybdisProtocol(void* h);
	// destructor
	~charybdisProtocol();

	void duringBurst();
	void initServer();
};

namespace charybdis
{
	charybdisProtocol* ircd; // use ircd to refer to instance's InspIRCdProtocol class
}

// initialize the ircd protocol
initIRCdProtocol(charybdisProtocol)

