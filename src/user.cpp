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
// $Id: user.cpp 706 2009-01-31 20:17:11Z ankit $
//===================================================//

#include "user.h"

/**
 User::User

 constructor
*/
User::User(nstring::str &u, nstring::str &n, nstring::str &i, nstring::str &h, nstring::str &rh, nstring::str& ip, nstring::str &m, nstring::str &g, nstring::str &s, std::time_t &ts) :
    uid(u),
    nick(n),
	oldNick(""),
    ident(i),
    host(h),
	oldHost(h),
    realHost(rh),
	ipAddress(ip),
	modes(m),
    gecos(g),
    server(s),
    timeStamp(ts),
    oper(false),
	identified(false),
    privs(""),
    accountName(""),
    usePrivmsg(false),
    language("") {}

/**
 User::~User

 destructor
*/
User::~User() {}
