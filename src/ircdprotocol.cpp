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
#include "utils.h"

#include <iostream>

IRCdProtocol::IRCdProtocol(void* h, const nstring::str ver) :
	handle(h),
	version(ver),
	requireNumeric(false),
	finishedBurst(false),
	duringBurst(false)
{ }

IRCdProtocol::~IRCdProtocol()
{
	std::map<nstring::str, IRCdCommand*>::iterator i;
	
	for (i = commands.begin(); i != commands.end(); ++i)
		delete i->second;
	commands.clear();
}

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

