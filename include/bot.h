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
// $Id: bot.h 710 2009-02-01 12:53:24Z ankit $
//===================================================//

#ifndef	BOT_H
#define	BOT_H

#include "user.h"

#include <deque>

class Service;

// a Bot is a special User that has a Service items
// which can make the Bot receive and respond to
// commands sent as PRIVMSG or NOTICE to it. each
// Service item processes this command.
class Bot : public User
{
public:
	// the bot's services
	std::deque<Service*> services;
	
	// constructor
	Bot();
	// destructor
	virtual ~Bot();
};

#endif // BOT_H

