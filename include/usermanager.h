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

#ifndef	USERMANAGER_H
#define	USERMANAGER_H

#include "base.h"
#include "user.h"

// manages users on the network
class UserManager
{
	// uid map
	std::map<nstring::str, nstring::str> uidMap;
	// internal user map
	std::map<nstring::str, User*> users;
	
public:

	// constructor
	UserManager();
	// destructor
	virtual ~UserManager();
	
	// handle connect
	void handleConnect(nstring::str&, nstring::str&, nstring::str&, nstring::str&, nstring::str&, nstring::str&, nstring::str&, nstring::str&, nstring::str&, nstring::str&, nstring::str&);
	// handle quit
	void handleQuit(nstring::str&);
	// handle nick
	void handleNick(nstring::str&, nstring::str&);
	// handle mode
	void handleMode(nstring::str&, nstring::str&);
	// handle host
	void handleHost(nstring::str&, nstring::str&);
	// handle oper
	void handleOper(nstring::str&, bool);
	
	// get nickname from id
	void getNickFromId(nstring::str&, nstring::str&);
	// get user from id
	User* getUserFromId(nstring::str&);
	// get user
	User* getUser(nstring::str&);
};

#endif // USERMANAGER_H

