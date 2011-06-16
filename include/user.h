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
// $Id: user.h 707 2009-01-31 21:52:15Z ankit $
//===================================================//

#ifndef	USER_H
#define	USER_H

#include "extensible.h"

class Server;

// a user on the network
class User : public Extensible
{
public:
	// nick of the user
	String nick;
	// ident of the user
	String ident;
	// vhost of the user
	String vHost;

	// constructor
	User();
	// destructor
	virtual ~User();
};

#endif // USER_H

