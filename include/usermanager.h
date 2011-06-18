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
// $Id: usermanager.h 706 2009-01-31 20:17:11Z ankit $
//===================================================//

#ifndef	USERMANAGER_H
#define	USERMANAGER_H

#include "base.h"
#include "user.h"

// manages users on the network
class UserManager
{
	// uid map
	std::map<nstring::str, nstring::str&> uidMap;
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
	
	// get nickname from id
	void getNickFromId(nstring::str&, nstring::str&);
	// get user
	//nstring::str GetUser(nstring::str&);
};

#endif // USERMANAGER_H

