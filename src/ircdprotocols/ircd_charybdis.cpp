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
// IRCD COMMANDS

class IRCdCommandInit : public IRCdCommand
{
	public:
	
		IRCdCommandInit() : IRCdCommand("NOTICE")
		{ }
		
		void execute(String&, String &params)
		{
			if (params == "* :*** Found your hostname")
				chary::ircd->sendBurst();
		}
};

class IRCdCommandPass : public IRCdCommand
{
	public:
	
		IRCdCommandPass() : IRCdCommand("PASS")
		{ }
		
		void execute(String&, String &params)
		{
			StringParser p(params);
			String param;
			
			while (p.GetToken(param, ' '))
				std::cout << param.c_str();
		}
};

class IRCdCommandCapab : public IRCdCommand
{
	public:

		IRCdCommandCapab() : IRCdCommand("CAPAB")
		{ }
		
		void execute(String&, String &params)
		{
			return instance->ircdProtocol->processBuffer(params, this);
		}
};

// SERVER METHODS

charybdisServer::charybdisServer()
{ }

charybdisServer::~charybdisServer()
{ }


// PROTOCOL METHODS

charybdisProtocol::charybdisProtocol(void* h)
: IRCdProtocol(h, "$Id: ircd_charybdis.cpp 707 2009-01-31 21:52:15Z ankit $")
{
	// we're using insp::ircd as a global variable so that
	// all IRCdCommands can make use of it
	chary::ircd = this;
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

	addCommand(new IRCdCommandInit);
	addCommand(new IRCdCommandPass);
	addCommand(new IRCdCommandCapab);
}


charybdisProtocol::~charybdisProtocol()
{ }


void charybdisProtocol::sendVersion()
{
	// :<sid> VERSION :<arbitrary version string>
	//instance->socketEngine->sendString(":" + sid + " VERSION :acora-" + config::VERSION + " (" + sid + ") " + name);
}


void charybdisProtocol::sendBurst()
{
	// PASS <password> TS 6 :<sid>
	instance->socketEngine->sendString("PASS " + instance->configReader->getValue<String>("remoteserver", "password", String()) + " TS 6 :" + sid);
	// SERVER <servername> <hopcount> :<description>
	instance->socketEngine->sendString("SERVER " + instance->configReader->getValue<String>("servicesserver", "name", String()) + " 0 :" + instance->configReader->getValue<String>("servicesserver", "desc", String()));

	// :sid BURST {timestamp}
	//instance->socketEngine->sendString(":" + sid + " BURST " + utils::toString<time_t>(instance->time()));
	// send version
	//sendVersion();

	// XXX introduce clients

	// :sid ENDBURST
	//instance->socketEngine->sendString(":" + sid + " ENDBURST");
}

