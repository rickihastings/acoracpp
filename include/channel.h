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

#ifndef CHANNEL_H
#define CHANNEL_H

#include "base.h"

// A channel on the network
struct Channel
{
	// name
	nstring::str name;
	// timestamp
	std::time_t time;
	
	// topic
	nstring::str topic;
	// topic setter
	nstring::str topicSetter;
	
	// modes
	nstring::str modes;
	// omodes (bans, exceptions, etc)
	std::map<nstring::str, nstring::str> oModes;
	// users (parsed)
	irc::nhash users;

	// constructor
	Channel(nstring::str&, std::time_t&);
	// destructor
	virtual ~Channel();
};

#endif // CHANNEL_H

