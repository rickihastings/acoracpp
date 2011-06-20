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

#ifndef	CHANNELMANAGER_H
#define	CHANNELMANAGER_H

#include "base.h"
#include "channel.h"

// manages channels on the network
class ChannelManager
{
	// internal channel map
	std::map<nstring::str, Channel*> chans;

public:

	// constructor
	ChannelManager();
	// destructor
	virtual ~ChannelManager();
	
	// parse user string
	std::map<nstring::str, nstring::str> parseUsers(std::vector<nstring::str>&);
	
	// get channel
	Channel* getChannel(nstring::str&);
	// handle chan create
	void handleCreate(nstring::str&, nstring::str&, nstring::str&, std::vector<nstring::str>&);
	// handle join
	void handleJoin(nstring::str&, nstring::str&, nstring::str&);
	// handle part
	void handlePart(nstring::str&, nstring::str&);
	// handle quit
	
	// handle nick changes (believe it or not)
	
	// handle mode
	
	// handle topic/ftopic
	
	// handle tb (TS6)
	
	// handle bmask (TS6)
	
};

#endif // CHANNELMANAGER_H

