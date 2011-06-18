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
#include "usermanager.h"
#include "channelmanager.h"

#include <iostream>
#include <algorithm>

// IRCD COMMANDS

// :<source> PING :<reply>
class IRCdCommandPing : public IRCdCommand
{
	public:
		IRCdCommandPing() : IRCdCommand("PING") { }

		void execute(nstring::str&, nstring::str &paramStr, std::vector<nstring::str> &params)
		{
			// :<sid> PONG :<reply>
			instance->socketEngine->sendString(":" + charybdis::ircd->sid + " PONG :" + params.at(0));
			
			if (instance->ircdProtocol->duringBurst)
			{
				instance->ircdProtocol->duringBurst = false;
				instance->ircdProtocol->finishedBurst = true;
			}
			// we recieve our first PING to tell us that the burst has finished.
		}
};

// :<source> NOTICE <target> :<message>
class IRCdCommandInit : public IRCdCommand
{
	public:
		IRCdCommandInit() : IRCdCommand("NOTICE") { }
		
		void execute(nstring::str&, nstring::str &paramStr, std::vector<nstring::str> &params)
		{
			if (paramStr == "* :*** Found your hostname")
				charybdis::ircd->initServer();
			// this is stupid but we have to do it unfortauntely..
		}
};

// PASS <pass> TS 6 <sid>
class IRCdCommandPass : public IRCdCommand
{
	public:
		IRCdCommandPass() : IRCdCommand("PASS") { }

		void execute(nstring::str&, nstring::str &paramStr, std::vector<nstring::str> &params)
		{
			charybdis::ircd->linkSid = params.at(3);
			// ok so we've recived the pass command. Let's take the id we get and store it
			utils::stripColon(charybdis::ircd->linkSid);
			// no doubt there is a : at the start, remove that.
		}
};

// SERVER <name> <hopcount> :<server gecos>
class IRCdCommandServer : public IRCdCommand
{
	public:
		IRCdCommandServer() : IRCdCommand("SERVER") { }

		void execute(nstring::str&, nstring::str &paramStr, std::vector<nstring::str> &params)
		{
			charybdis::ircd->linkName = params.at(0);
			// ok so we've recived the server command. Let's take the id we get and store it
			
			nstring::str time;
			utils::toStr(time, instance->now);
			// :<sid> SVINFO 6 6 0 <timestamp>
			instance->socketEngine->sendString(":" + charybdis::ircd->sid + " SVINFO 6 6 0 " + time);
		}
};

// CAPAB :<capabilities>
class IRCdCommandCapab : public IRCdCommand
{
	public:
		IRCdCommandCapab() : IRCdCommand("CAPAB") { }
		
		void execute(nstring::str&, nstring::str &paramStr, std::vector<nstring::str> &params)
		{
			std::vector<nstring::str>::iterator it;
			std::sort(params.begin(), params.end());
			
			if (!std::binary_search(params.begin(), params.end(), "SERVICES"))
				instance->finalize("(ircd_charybdis) Required capability SERVICES not found.");
			// search CAPAB to see if they support SERVICES, if not bail!
		}
};

// SVINFO 6 6 0 :<timestamp>
class IRCdCommandSVINFO : public IRCdCommand
{
	public:
		IRCdCommandSVINFO() : IRCdCommand("SVINFO") { }
		
		void execute(nstring::str&, nstring::str &paramStr, std::vector<nstring::str> &params)
		{
			instance->ircdProtocol->duringBurst = true;
			// we recieve SVINFO upon bursts, this is how we know a burst is definately starting.
		}
};

// :<source> EUID <nick> 0 <nickts> <umodes> <username> <visible hostname> <ip> <uid> <real hostname> <account name> :<gecos>
class IRCdCommandEUID : public IRCdCommand
{
	public:
		IRCdCommandEUID() : IRCdCommand("EUID") { }
		
		void execute(nstring::str &src, nstring::str &paramStr, std::vector<nstring::str> &params)
		{
			nstring::str gecos = utils::getDataAfter(params, 10);
			utils::stripColon(gecos);
			// gecos probably has a :, let's remove it for our buddy userManager
			
			instance->userManager->handleConnect(params.at(0), params.at(4), params.at(5), params.at(8), params.at(6), params.at(9), params.at(2), params.at(7), params.at(3), src, gecos);
			// we send user manager some information about the user we just recieved. like so;
			// > nick, username, hostname, real hostname, ip address, account name, nickts, uid, umodes, src, gecos
		}
};

// :<source> QUIT :<quit message>
class IRCdCommandQuit : public IRCdCommand
{
	public:
		IRCdCommandQuit() : IRCdCommand("QUIT") { }
		
		void execute(nstring::str &src, nstring::str &paramStr, std::vector<nstring::str> &params)
		{
			instance->userManager->handleQuit(src);
			// we send the uid. into handleQuit
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
	// throw an exception if the numeric isnt correct.
	
	// read our name
	instance->configReader->getValue(name, "servicesserver", "name");

	// add commands.
	addCommand(new IRCdCommandInit);
	addCommand(new IRCdCommandPass);
	addCommand(new IRCdCommandServer);
	addCommand(new IRCdCommandCapab);
	addCommand(new IRCdCommandSVINFO);
	addCommand(new IRCdCommandEUID);
	addCommand(new IRCdCommandQuit);
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
	instance->socketEngine->sendString("PASS " + instance->configReader->getValue<nstring::str>("remoteserver", "password", nstring::str()) + " TS 6 :" + sid);
	// CAPAB :<supported capab>
	instance->socketEngine->sendString("CAPAB :QS EX CHW IE KLN KNOCK TB UNKLN CLUSTER ENCAP SERVICES RSFNC SAVE EUID EOPMOD BAN MLOCK");
	// SERVER <servername> <hopcount> :<description>
	instance->socketEngine->sendString("SERVER " + instance->configReader->getValue<nstring::str>("servicesserver", "name", nstring::str()) + " 0 :" + instance->configReader->getValue<nstring::str>("servicesserver", "desc", nstring::str()));
}
