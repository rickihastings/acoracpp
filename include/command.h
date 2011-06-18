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
// $Id: command.h 692 2009-01-29 09:15:03Z ankit $
//===================================================//

#ifndef	COMMAND_H
#define	COMMAND_H

#include "base.h"

// base class for commands
class Command
{
public:
	// command name
	const nstring::str name;

	// consturctor
	Command(const nstring::str);
	// destructor
	virtual ~Command();

	// execute the command
	virtual ErrorCode execute(nstring::str&) = 0;
};

#endif // COMMAND_H

