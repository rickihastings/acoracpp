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
// $Id: ircd_charybdis.cpp 707 2009-01-31 21:52:15Z ankit $
//===================================================//

#include "ircd_charybdis.h"

#include "instance.h"
#include "stringparser.h"
#include "configreader.h"
#include "socketengine.h"
#include "config.h"
#include "exception.h"

#include <iostream>
#include <algorithm>

// IRCD COMMANDS

// PING
class IRCdCommandPing : public IRCdCommand
{
	public:
		IRCdCommandPing() : IRCdCommand("PING") { }

		void execute(String&, String &paramStr, std::vector<String> &params)
		{
			// :<sid> PONG :<reply>
			instance->socketEngine->sendString(":" + charybdis::ircd->sid + " PONG :" + params.at(0));
		}
};

// NOTICE (hacked)
class IRCdCommandInit : public IRCdCommand
{
	public:
		IRCdCommandInit() : IRCdCommand("NOTICE") { }
		
		void execute(String&, String &paramStr, std::vector<String> &params)
		{
			if (paramStr == "* :*** Found your hostname")
				charybdis::ircd->initServer();
			// this is stupid but we have to do it unfortauntely..
		}
};

// PASS
class IRCdCommandPass : public IRCdCommand
{
	public:
		IRCdCommandPass() : IRCdCommand("PASS") { }

		void execute(String&, String &paramStr, std::vector<String> &params)
		{
			charybdis::ircd->linkSid = params.at(3);
			// ok so we've recived the pass command. Let's take the id we get and store it
			if (*(charybdis::ircd->linkSid.begin()) == ':')
				charybdis::ircd->linkSid.erase(charybdis::ircd->linkSid.begin());
			// no doubt there is a : at the start, remove that.
		}
};

// 
class IRCdCommandServer : public IRCdCommand
{
	public:
		IRCdCommandServer() : IRCdCommand("SERVER") { }

		void execute(String&, String &paramStr, std::vector<String> &params)
		{
			charybdis::ircd->linkName = params.at(0);
			// ok so we've recived the server command. Let's take the id we get and store it
			
			String time;
			utils::toStr(time, instance->now);
			// :<sid> SVINFO 6 6 0 <timestamp>
			instance->socketEngine->sendString(":" + charybdis::ircd->sid + " SVINFO 6 6 0 " + time);
		}
};

// CAPAB
class IRCdCommandCapab : public IRCdCommand
{
	public:
		IRCdCommandCapab() : IRCdCommand("CAPAB") { }
		
		void execute(String&, String &paramStr, std::vector<String> &params)
		{
			std::vector<String>::iterator it;
			std::sort(params.begin(), params.end());
			
			if (!std::binary_search(params.begin(), params.end(), "SERVICES"))
				instance->finalize("(ircd_charybdis) Required capability SERVICES not found.");
			// search CAPAB to see if they support SERVICES, if not bail!
		}
};

// SERVER METHODS

charybdisServer::charybdisServer()
{ }

charybdisServer::~charybdisServer()
{ }


// PROTOCOL METHODS

charybdisProtocol::charybdisProtocol(void* h)
: IRCdProtocol(h, "$Id: ircd_charybdis.cpp 707 2009-01-31 21:52:15Z ricki $")
{
	// we're using charybdis::ircd as a global variable so that
	// all IRCdCommands can make use of it
	charybdis::ircd = this;
	// inspircd requires numeric to be specified
	requireNumeric = true;

	// read our numeric
	instance->configReader->getValue(sid, "servicesserver", "numeric");
	
	// if no sid is mentioned, don't load
	if (sid.empty())
		throw Exception("ircd_charybdis", "ServicesServer::numeric is not specified.");

	else if (	sid.length() != 3 || !(sid[0] >= '0' && sid[0] <= '9')
			|| !((sid[1] >= 'A' && sid[1] <= 'Z') || (sid[1] >= '0' && sid[1] <= '9'))
			|| !((sid[2] >= 'A' && sid[2] <= 'Z') || (sid[2] >= '0' && sid[2] <= '9'))
	)
		throw Exception("ircd_charybdis", "ServicesServer::numeric must be in the format [0-9][A-Z0-9][A-Z0-9].");
	
	// read our name
	instance->configReader->getValue(name, "servicesserver", "name");

	// add commands.
	addCommand(new IRCdCommandInit);
	addCommand(new IRCdCommandPass);
	addCommand(new IRCdCommandServer);
	addCommand(new IRCdCommandCapab);
	addCommand(new IRCdCommandPing);
}


charybdisProtocol::~charybdisProtocol()
{ }

void charybdisProtocol::duringBurst()
{
	// TODO INTRODUCE CLIENTS
	
}

void charybdisProtocol::initServer()
{
	// PASS <password> TS 6 :<sid>
	instance->socketEngine->sendString("PASS " + instance->configReader->getValue<String>("remoteserver", "password", String()) + " TS 6 :" + sid);
	// CAPAB :<supported capab>
	instance->socketEngine->sendString("CAPAB :QS EX CHW IE KLN KNOCK TB UNKLN CLUSTER ENCAP SERVICES RSFNC SAVE EUID EOPMOD BAN MLOCK");
	// SERVER <servername> <hopcount> :<description>
	instance->socketEngine->sendString("SERVER " + instance->configReader->getValue<String>("servicesserver", "name", String()) + " 0 :" + instance->configReader->getValue<String>("servicesserver", "desc", String()));
}
