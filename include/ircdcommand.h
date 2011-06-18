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

#include "base.h"

// base class for IRCd commands
class IRCdCommand
{
public:
	// command name
	const nstring::str name;

	// constructor
	IRCdCommand(const nstring::str);
	// destructor
	virtual ~IRCdCommand();

	// execute the command
	virtual void execute(nstring::str &src, nstring::str &paramStr, std::vector<nstring::str> &params) = 0;
};

#endif // IRCDCOMMAND_H

