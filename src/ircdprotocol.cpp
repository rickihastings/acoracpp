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
#include "utils.h"

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
	String cmd, paramStr;
	
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
		p.GetRemaining(paramStr);
	// find out if we have a matching command, if not bail
	
	std::vector<String> params;
	
	if (*(paramStr.begin()) == ':')
		paramStr.erase(paramStr.begin());
		
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

