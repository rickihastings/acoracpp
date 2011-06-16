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
// $Id: ircdprotocol.cpp 698 2009-01-29 16:33:02Z ankit $
//===================================================//

#include "ircdprotocol.h"
#include "ircdcommand.h"
#include "instance.h"
#include "stringparser.h"


IRCdProtocol::IRCdProtocol(void* h, const String ver) :
	handle		(h),
	version		(ver),
	requireNumeric	(false)
{ }


IRCdProtocol::~IRCdProtocol()
{ }


void IRCdProtocol::processBuffer(String &buf, IRCdCommand* parent, String src)
{
	bool getsrc = false;
	String cmd, params;
	
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
	
	std::map<String, IRCdCommand*>::iterator i = commands.find(cmd);
	
	if (i == commands.end())
	{
		instance->debug("Command " + cmd + " not found");
		return;
	}
	else	
		p.GetRemaining(params);
	
	if (*(params.begin()) == ':')
		params.erase(params.begin());
	
	i->second->execute(src, params);
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

