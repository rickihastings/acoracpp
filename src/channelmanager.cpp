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

#include "instance.h"
#include "ircdprotocol.h"
#include "utils.h"
#include "modeparser.h"
#include "channelmanager.h"
#include "usermanager.h"

#include <iostream>

/**
 ChannelManager::ChannelManager

 constructor
*/
ChannelManager::ChannelManager()
{ }

/**
 ChannelManager::~ChannelManager

 destructor
*/
ChannelManager::~ChannelManager()
{
	std::map<nstring::str, Channel*>::iterator i;
	for (i = chans.begin(); i != chans.end(); ++i)
		delete i->second;
	chans.clear();
}

/**
 ChannelManager::parseUsers

 parse: @+42XAAAABG @42XAAAABF into Nickname => ov, OtherNick => o
 also parse: ov,42XAAAABG o,42XAAAABG into Nickname => ov, OtherNick => o
*/
std::map<nstring::str, nstring::str> ChannelManager::parseUsers(std::vector<nstring::str>& users)
{
	std::map<nstring::str, nstring::str> parsedUsers;
	std::vector<nstring::str> split;
	nstring::str tempNick, tempModes, newModes, tempPrefix, tempSuffix;
	if (users.empty())
		return parsedUsers;
	// setup a new container, if users is empty just return our empty container
	
	std::vector<nstring::str>::iterator it;
	for (it = users.begin(); it != users.end(); ++it)
	{
		if ((*it).empty())
			continue;
		// just skip if we've got doggo info
		
		if (utils::stringIsAlphaNumerical((*it)))
		{
			instance->userManager->getNickFromId(*it, tempNick);
			parsedUsers.insert(std::pair<nstring::str, nstring::str>(tempNick, ""));
			continue;
			// just a normal user with no @ or +, etc.
		}
		
		if ((*it).find(",") == std::string::npos)
		{
			tempNick = tempModes = (*it);
			tempNick.erase(std::remove_if(tempNick.begin(), tempNick.end(), utils::isNotAlnum<char>), tempNick.end());
			instance->userManager->getNickFromId(tempNick, tempNick);
			// get the nick
			
			tempModes.erase(std::remove_if(tempModes.begin(), tempModes.end(), isalnum), tempModes.end());
			for (nstring::str::iterator i = tempModes.begin(); i != tempModes.end(); ++i)
			{
				tempPrefix = (*i);
				tempSuffix = instance->ircdProtocol->prefixModes.at(*i);
				utils::replace(tempModes, tempPrefix, tempSuffix);
			}
			// get the prefix and convert them, ie @ to o, + to v
			
			parsedUsers.insert(std::pair<nstring::str, nstring::str>(tempNick, tempModes));
			continue;
			// remove all BUT alphanumerical characters, which is our UID.
		}
		// if we don't have a , let's just parse it!
	
		utils::explode(",", *it, split);
		instance->userManager->getNickFromId(split.at(1), tempNick);
		newModes = split.at(0);
		// get the nick etc
		
		for (nstring::str::iterator i = newModes.begin(); i != newModes.end(); ++i)
		{
			tempPrefix = (*i);
			tempSuffix = instance->ircdProtocol->prefixModes.at(*i);
			utils::replace(newModes, tempPrefix, tempSuffix);
		}
		// replace @ with o, + with v etc
		
		parsedUsers.insert(std::pair<nstring::str, nstring::str>(tempNick, newModes));
		// we do have a "," so parse it and fire it into parsedUsers.
	}
	// loop through users
	
	return parsedUsers;
}

/**
 ChannelManager::getChannel

 return channel from name
*/
Channel* ChannelManager::getChannel(nstring::str &chan)
{
	nstring::str uchan = chan;
	std::transform(uchan.begin(), uchan.end(), uchan.begin(), ::tolower);
	// to lower it, seen as though everything in chans is tolower.
	
	std::map<nstring::str, Channel*>::iterator it = chans.find(uchan);
	if (it != chans.end())
	{
		return it->second;
	}
	else
	{
		instance->log(ERROR, "getChannel(): trying to find invalid channel, this can cause serious issues!");
		return NULL;
	}
	// find the channel class from our channel map
}

/**
 ChannelManager::handleCreate

 handle SJOINS or FJOIN w/e
*/
void ChannelManager::handleCreate(nstring::str &chan, nstring::str &ts, nstring::str &modes, std::vector<nstring::str> &users)
{
	Channel* channel = NULL;
	std::map<nstring::str, Channel*>::iterator it;
	std::map<nstring::str, nstring::str> parsedUsers = parseUsers(users);
	std::map<nstring::str, nstring::str> oldUsers;
	// parse up users
	
	std::time_t timeStamp;
	std::istringstream stream(ts.c_str());
	stream >> timeStamp;
	// get the channel timestamp
	
	nstring::str uchan = chan;
	std::transform(uchan.begin(), uchan.end(), uchan.begin(), ::tolower);
	it = chans.find(chan);
	// ::tolower chan
	
	if (it == chans.end())
	{
		channel = new Channel(chan, timeStamp);
		chans.insert(std::pair<nstring::str, Channel*>(uchan, channel));
		channel->users.insert(parsedUsers.begin(), parsedUsers.end());
		
		instance->log(MISC, "handleCreate(): " + chan + " introduced to the network with a timestamp of " + ts);
		// log things, ie NETWORK
	}
	else
	{
		oldUsers = channel->users;
		channel->users.clear();
		
		parsedUsers.insert(oldUsers.begin(), oldUsers.end());
		channel->users.insert(parsedUsers.begin(), parsedUsers.end());
		// set some vars in our Channel*
	}
	// create a new channel.
	
	irc::modes modeContainer;
	irc::params paramContainer;
	instance->modeParser->sortModes(modes, modeContainer, paramContainer, true);
	instance->modeParser->saveModes(channel, modeContainer, paramContainer);
	// parse modes and save modes..
}

/**
 ChannelManager::handleJoin

 handle JOINS
*/
void ChannelManager::handleJoin(nstring::str &uid, nstring::str &ts, nstring::str &chan)
{
	Channel* channel = getChannel(chan);
	nstring::str nick;
	instance->userManager->getNickFromId(uid, nick);
	// get the nick & channel
	
	channel->users.insert(std::pair<nstring::str, nstring::str>(nick, ""));
	
	instance->log(MISC, "handleJoin(): " + nick + " has joined " + chan);
	// log things, ie NETWORK
}

/**
 ChannelManager::handlePart

 handle PART
*/
void ChannelManager::handlePart(nstring::str &uid, nstring::str &chan)
{
	Channel* channel = getChannel(chan);
	std::map<nstring::str, Channel*>::iterator i = chans.find(chan);
	nstring::str nick;
	instance->userManager->getNickFromId(uid, nick);
	// get the nick & channel
	
	std::map<nstring::str, nstring::str>::iterator it = channel->users.find(nick);
	if (it == channel->users.end())
	{
		instance->log(ERROR, "handlePart(): trying to find user in channel map on part, can't find user!");
		return;
	}
	// can't find user in channel->users. major issue?
	
	channel->users.erase(it);
	
	if (channel->users.empty())
	{
		delete channel;
		chans.erase(i);
	}
	// remove the user from our internal channel->users array, ALSO
	// check if our internal array matches 0, if it does the channel
	// is empty so delete it.
	
	instance->log(MISC, "handlePart(): " + nick + " has left " + chan);
	// log things, ie NETWORK
}

/**
 ChannelManager::handleMode

 handle TMODE changes, etc.
*/
void ChannelManager::handleMode(nstring::str &uid, nstring::str &chan, nstring::str &modes)
{
	irc::modes modeContainer;
	irc::params paramContainer;
	instance->modeParser->sortModes(modes, modeContainer, paramContainer, true);
	// parse modes

	Channel* channel = getChannel(chan);
	instance->modeParser->saveModes(channel, modeContainer, paramContainer);
	// update the modes in our channel record
}
