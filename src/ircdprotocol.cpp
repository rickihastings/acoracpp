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

#include "ircdprotocol.h"
#include "ircdcommand.h"
#include "instance.h"
#include "stringparser.h"
#include "server.h"
#include "utils.h"

/**
 IRCdProtocol::IRCdProtocol

 constructor
*/
IRCdProtocol::IRCdProtocol(void* h, const nstring::str ver) :
	handle(h),
	version(ver),
	requireNumeric(false),
	finishedBurst(false),
	duringBurst(false)
{ }

/**
 IRCdProtocol::~IRCdProtocol

 destructor
*/
IRCdProtocol::~IRCdProtocol()
{
	std::map<nstring::str, IRCdCommand*>::iterator i;
	
	for (i = commands.begin(); i != commands.end(); ++i)
		delete i->second;
	commands.clear();
	// delete commands
	
	std::map<nstring::str, Server*>::iterator it;
	
	for (it = servers.begin(); it != servers.end(); ++it)
		delete it->second;
	servers.clear();
	// delete servers
}

/**
 IRCdProtocol::processBuffer

 socketEngine sends buffer to here to be processed
*/
void IRCdProtocol::processBuffer(nstring::str &buf, IRCdCommand* parent, nstring::str src)
{
	bool getsrc = false;
	nstring::str cmd, paramStr;
	
	if (!parent)
	{
		if (*(buf.begin()) == ':')
		{
			buf.erase(buf.begin());
			getsrc = true;
		}
	}
	
	StringParser p(buf);
	
	if (getsrc)
		p.GetToken(src);
	src = utils::trim(src, nstring::str(" "));
	// get src
	
	p.GetToken(cmd);
	if (parent)
		cmd = parent->name + " " + cmd;
	
	std::map<nstring::str, IRCdCommand*>::iterator i = commands.find(cmd);
	
	if (i == commands.end())
	{
		instance->log(ERROR, "IRCdProtocol(): Command " + cmd + " not found, not critical.");
		return;
	}
	else
		p.GetRemaining(paramStr);
	// find out if we have a matching command, if not bail
	
	std::vector<nstring::str> params;
	
	utils::stripColon(paramStr);
	// strip the colon!
		
	utils::explode(" ", paramStr, params);
	// take the : off params, split it via ' ', send it into the execute function
	
	i->second->execute(src, paramStr, params);
}

/**
 IRCdProtocol::addCommand

 function to add ircd commands.
*/
ErrorCode IRCdProtocol::addCommand(IRCdCommand* command)
{
	if (commands.find(command->name) == commands.end())
	{
		commands[command->name] = command;
		return err::ircdprotocol::none;
	}

	delete command;
	return err::ircdprotocol::alreadyExists;
}

/**
 IRCdProtocol::addServer

 function to add servers
*/
void IRCdProtocol::addServer(nstring::str &server, nstring::str &sid)
{
	servers.insert(std::pair<nstring::str, Server*>(sid, new Server(server, sid)));
	
	instance->log(NETWORK, "addServer(): " + server + " has connected to the network with the server id: " + sid);
	//instance->log(LOGCHAN, "// TODO");
	// log things, ie LOGCHAN and NETWORK
}

/**
 IRCdProtocol::remServer

 function to remove servers
*/
void IRCdProtocol::remServer(nstring::str &sid)
{
	nstring::str server;
	std::map<nstring::str, Server*>::iterator it = servers.find(sid);
	
	if (it != servers.end())
	{
		server = it->second->name;
		delete it->second;
	}
	else
		instance->log(ERROR, "remServer(): cannot find server in internal server array, something has went WRONG!");
	// find the server, and remove it.
	servers.erase(it);
	
	instance->log(NETWORK, "remServer(): " + server + ":" + sid + " has disconnected from the network");
	//instance->log(LOGCHAN, "// TODO");
	// log things, ie LOGCHAN and NETWORK
}

/**
 IRCdProtocol::getServerFromId

 function to get servers from ids
*/
nstring::str IRCdProtocol::getServerFromId(nstring::str &sid)
{
	nstring::str name = "";
	std::map<nstring::str, Server*>::iterator it = servers.find(sid);
	
	if (it != servers.end())
		name = it->second->name;
	else
		instance->log(ERROR, "getServerFromId(): cannot find server in internal server array from id.");
	// set name as name from server class
	
	return name;
}

/**
 IRCdProtocol::parseModes

 function to parse modes from a format like 'eIb,k,flj,CFPcgimnpstz'
*/
void IRCdProtocol::parseModes(nstring::str &m)
{
	std::vector<nstring::str> parts, parsedPrefix;
	nstring::str modeString, prefixModesS, prefixModesA;
	modeString = m;
	utils::explode(",", modeString, parts);
	
	restrictModes = parts.at(0);
	modesWithUParams = parts.at(2);
	
	if (restrictModes.find("q") != std::string::npos)
	{
		owner = true;
		statusModes += "q";
		utils::replace(restrictModes, nstring::str("q"), nstring::str(""));
		// remove from restrictModes and add to statusModes in order
	}
	
	if (restrictModes.find("a") != std::string::npos)
	{
		owner = true;
		statusModes += "a";
		utils::replace(restrictModes, nstring::str("a"), nstring::str(""));
		// remove from restrictModes and add to statusModes in order
	}
	// search restrict modes for 'aq' as we want them in status modes
	
	prefixModesS = prefixData;
	utils::explode(")", prefixModesS, parsedPrefix);
	prefixModesS = parsedPrefix.at(0).substr(1);
	prefixModesA = parsedPrefix.at(1);
	statusModes += prefixModesS;
	// parse (ov)@+, we only really want "ov" right now
	
	modesWithParams = statusModes + restrictModes + parts.at(1) + modesWithUParams;
	modes = modesWithParams + parts.at(3);
	// compile our mode strings.
	
	if (prefixModesA.find("q") != std::string::npos)
		prefixModes.insert(std::pair<char, char>('q', '@'));
	if (prefixModesA.find("a") != std::string::npos)
		prefixModes.insert(std::pair<char, char>('a', '@'));
	// we assume that qa = @ when we aint told any better
	
	int part = 0;
	for (nstring::str::iterator is = prefixModesS.begin(); is != prefixModesS.end(); ++is)
	{
		prefixModes.insert(std::pair<char, char>(prefixModesA.at(part), prefixModesS.at(part)));
		part++;
	}
	// parse (ov)@+ into a map. ie @ = o, + = v	etc
}
