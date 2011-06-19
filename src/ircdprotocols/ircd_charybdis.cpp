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

#include "ircd_charybdis.h"

#include "instance.h"
#include "stringparser.h"
#include "configreader.h"
#include "socketengine.h"
#include "config.h"
#include "exception.h"
#include "usermanager.h"
#include "channelmanager.h"
#include "modeparser.h"

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
class IRCdCommandNotice : public IRCdCommand
{
	public:
		IRCdCommandNotice() : IRCdCommand("NOTICE") { }
		
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
			// ok so we've recived the pass command. Let's take the id we get and store it (this only stores the last link)
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
			// ok so we've recived the server command. Let's take the id we get and store it (this only stores the last link)
			
			charybdis::ircd->addServer(charybdis::ircd->linkName, charybdis::ircd->linkSid);
			// send data to the core
			
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
			charybdis::ircd->parseModes(charybdis::ircd->capabModes);
			// parse modes
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

// SQUIT <sid> :<quit message>
class IRCdCommandSQuit : public IRCdCommand
{
	public:
		IRCdCommandSQuit() : IRCdCommand("SQUIT") { }

		void execute(nstring::str&, nstring::str &paramStr, std::vector<nstring::str> &params)
		{
			charybdis::ircd->remServer(params.at(0));
			// send data to the core
		}
};

// ERROR :<message>
class IRCdCommandError : public IRCdCommand
{
	public:
		IRCdCommandError() : IRCdCommand("ERROR") { }
		
		void execute(nstring::str&, nstring::str &paramStr, std::vector<nstring::str> &params)
		{
			instance->log(ERROR, "processBuffer(): " + paramStr);
			// log this, quite important.
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

// :<source> NICK <nick> :<timestamp>
class IRCdCommandNick : public IRCdCommand
{
	public:
		IRCdCommandNick() : IRCdCommand("NICK") { }
		
		void execute(nstring::str &src, nstring::str &paramStr, std::vector<nstring::str> &params)
		{
			instance->userManager->handleNick(src, params.at(0));
			// we send the uid. into handleQuit
		}
};

// :<source> MODE <nick> :<modes>
class IRCdCommandUMode : public IRCdCommand
{
	public:
		IRCdCommandUMode() : IRCdCommand("MODE") { }
		
		void execute(nstring::str&, nstring::str &paramStr, std::vector<nstring::str> &params)
		{
			nstring::str modes = params.at(1);
			utils::stripColon(modes);
			instance->userManager->handleMode(params.at(0), modes);
			// we send the new modes into handleMode, which is parsed by mode class.
			
			irc::modes modeContainer;
			irc::params paramContainer;
			instance->modeParser->sortModes(modes, modeContainer, paramContainer, false);
			// parse modes and send them into the user manager // TODO
			
			if (modeContainer["plus"].find('o') != std::string::npos)
				instance->userManager->handleOper(params.at(0), true);
			// check if we have an "+o", if we do call handleOper()
			if (modeContainer["minus"].find('o') != std::string::npos)
				instance->userManager->handleOper(params.at(0), false);
			// check if we have a "-o", if we do again call handleOper()
		}
};

// :<source> CHGHOST <uid> <new host>
class IRCdCommandCHGHost : public IRCdCommand
{
	public:
		IRCdCommandCHGHost() : IRCdCommand("CHGHOST") { }
		
		void execute(nstring::str &src, nstring::str &paramStr, std::vector<nstring::str> &params)
		{
			instance->userManager->handleHost(params.at(0), params.at(1));
			// we send the uid. into handleQuit
		}
};

// :<uid> SJOIN <timestamp> <chan> <modes params> :<users>
class IRCdCommandSJoin : public IRCdCommand
{
	public:
		IRCdCommandSJoin() : IRCdCommand("SJOIN") { }
		
		void execute(nstring::str&, nstring::str &paramStr, std::vector<nstring::str> &params)
		{
			std::vector<nstring::str> split, pSplit, users;
			utils::explode(":", paramStr, split);
			utils::explode(" ", split.at(0), pSplit);
			utils::explode(" ", split.at(1), users);
			pSplit.erase(pSplit.begin(), pSplit.begin()+2);
			nstring::str modes = utils::getDataAfter(pSplit, 0);
			// explode via : to seperate modes etc from users
			
			instance->channelManager->handleCreate(params.at(1), params.at(0), modes, users);
			// send data to channel manager
		}
};

// SERVER METHODS

charybdisServer::charybdisServer() { }

charybdisServer::~charybdisServer() { }

// PROTOCOL METHODS

charybdisProtocol::charybdisProtocol(void* h)
: IRCdProtocol(h, "$Id: ircd_charybdis.cpp 707 2009-01-31 21:52:15Z ricki $")
{
	// we're using charybdis::ircd as a global variable so that
	// all IRCdCommands can make use of it
	charybdis::ircd = this;
	// inspircd requires numeric to be specified
	requireNumeric = true;
	// max params (4 in charybdis i think it is) ?
	maxParams = 4;
	capabModes = "eIb,k,flj,CFPcgimnpstz";
	prefixData = "(ov)@+";
	defaultChanModes = "+nt";
	owner = protect = halfop = false;

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
	
	addServer(name, sid);
	// add our server.

	// add commands.
	addCommand(new IRCdCommandNotice);
	addCommand(new IRCdCommandPass);
	addCommand(new IRCdCommandServer);
	addCommand(new IRCdCommandCapab);
	addCommand(new IRCdCommandSVINFO);
	addCommand(new IRCdCommandSQuit);
	addCommand(new IRCdCommandError);
	addCommand(new IRCdCommandEUID);
	addCommand(new IRCdCommandQuit);
	addCommand(new IRCdCommandNick);
	addCommand(new IRCdCommandUMode);
	addCommand(new IRCdCommandCHGHost);
	addCommand(new IRCdCommandSJoin);
	addCommand(new IRCdCommandPing);
}

charybdisProtocol::~charybdisProtocol() { }

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
