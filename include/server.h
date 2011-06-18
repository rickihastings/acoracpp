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

#ifndef SERVER_H
#define SERVER_H

#include "base.h"

// a server on the network
class Server
{
public:

	// name
	nstring::str name;
	// sid
	nstring::str sid;

	// constructor
	Server(nstring::str&, nstring::str&);
	// destructor
	virtual ~Server();
};

#endif // SERVER_H

