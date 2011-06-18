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

#ifndef	USER_H
#define	USER_H

#include "base.h"

/**
 User

 user struct
*/
struct User
{
    // id
    int id;
    // uid
    nstring::str uid;
    // nick
    nstring::str nick;
    // ident
    nstring::str ident;
    // host
    nstring::str host;
    // old host
    nstring::str oldHost;
	// real host
	nstring::str realHost;
	// ip address
	nstring::str ipAddress;
	
	// modes
	nstring::str modes;
    // gecos
    nstring::str gecos;
    // server
    nstring::str server;
    // connect timestamp
    std::time_t timeStamp;
	
    // oper
    bool oper;
	// identified
	bool identified;
    // oper privs
    nstring::str privs;

    // account name
    nstring::str accountName;
    // use privmsg
    bool usePrivmsg;
    // language
    nstring::str language;	

    // constructor
	User(nstring::str&, nstring::str&, nstring::str&, nstring::str&, nstring::str&, nstring::str&, nstring::str&, nstring::str&, nstring::str&, std::time_t&);
    // destructor
    virtual ~User();
};

#endif // USER_H

