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

#include "extensible.h"

// base class for commands
class Command : public Extensible
{
public:
	// command name
	const String name;

	// consturctor
	Command(const String);
	// destructor
	virtual ~Command();

	// execute the command
	virtual ErrorCode execute(String&) = 0;
};

#endif // COMMAND_H

