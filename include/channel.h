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
// $Id: channel.h 705 2009-01-31 19:51:10Z ankit $
//===================================================//

#ifndef	CHANNEL_H
#define	CHANNEL_H

#include "extensible.h"

// a channel on the network
class Channel : public Extensible
{
public:
	// channel name
	const String name;
	
	// constructor
	Channel(const String);
	// destructor
	virtual ~Channel();
};

#endif // CHANNEL_H

