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
// $Id: ircdcommand.h 706 2009-01-31 20:17:11Z ankit $
//===================================================//

#ifndef IRCDCOMMAND_H
#define IRCDCOMMAND_H

#include "extensible.h"

// base class for IRCd commands
class IRCdCommand : public Extensible
{
public:
	// command name
	const String name;

	// constructor
	IRCdCommand(const String);
	// destructor
	virtual ~IRCdCommand();

	// execute the command
	virtual void execute(String &src, String &paramStr, std::vector<String> &params) = 0;
};

#endif // IRCDCOMMAND_H

