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
	std::map<nstring::str, Channel*>::iterator it = chans.find(chan);
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
	it = chans.find(uchan);
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
	
	User* user = NULL;
	nstring::str tempNick;
	for (std::map<nstring::str, nstring::str>::iterator pit = parsedUsers.begin(); pit != parsedUsers.end(); ++pit)
	{
		tempNick = pit->first;
		user = instance->userManager->getUser(tempNick);
		user->qChans.push_back(uchan);
		// get the nick & channel
	}
	// for channel->users and update the user's qChan vector
}

/**
 ChannelManager::handleJoin

 handle JOINS
*/
void ChannelManager::handleJoin(nstring::str &uid, nstring::str &ts, nstring::str &chan)
{
	nstring::str uchan = chan;
	std::transform(uchan.begin(), uchan.end(), uchan.begin(), ::tolower);
	// to lower chan
	
	Channel* channel = getChannel(uchan);
	nstring::str nick;
	
	instance->userManager->getNickFromId(uid, nick);
	User* user = instance->userManager->getUser(nick);
	// get the nick & channel
	
	user->qChans.push_back(uchan);
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
	nstring::str uchan = chan;
	std::transform(uchan.begin(), uchan.end(), uchan.begin(), ::tolower);
	// to lower chan.

	std::map<nstring::str, Channel*>::iterator i = chans.find(chan);
	nstring::str nick;
	std::vector<nstring::str>::iterator qit;
	
	instance->userManager->getNickFromId(uid, nick);
	User* user = instance->userManager->getUser(nick);
	// get the nick & channel
	
	std::map<nstring::str, nstring::str>::iterator it = i->second->users.find(nick);
	if (it == i->second->users.end())
	{
		instance->log(ERROR, "handlePart(): trying to find user in channel map on part, can't find user!");
		return;
	}
	// can't find user in channel->users. major issue?
	
	i->second->users.erase(it);
	if (i->second->users.empty())
	{
		delete i->second;
		chans.erase(i);
	}
	// remove the user from our internal channel->users array, ALSO
	// check if our internal array matches 0, if it does the channel
	// is empty so delete it.
	
	qit = std::find(user->qChans.begin(), user->qChans.end(), uchan);
	if (qit != user->qChans.end())
		user->qChans.erase(qit);
	// the user struct also contains a method to quickly see what channels a user is in
	// compared to searching every channel, AND every user array at the same time.
	
	instance->log(MISC, "handlePart(): " + nick + " has left " + chan);
	// log things, ie NETWORK
}

/**
 ChannelManager::handleQuit

 usually we wouldn't have to do anything upon QUIT, but we need to remove
 the quitting user from all channel arrays..
*/
void ChannelManager::handleQuit(nstring::str &uid)
{
	nstring::str nick;
	instance->userManager->getNickFromId(uid, nick);
	User* user = instance->userManager->getUser(nick);
	// get the user.
	
	for (std::vector<nstring::str>::iterator i = user->qChans.begin(); i != user->qChans.end(); ++i)
	{
		std::map<nstring::str, Channel*>::iterator cit = chans.find(*i);
		std::map<nstring::str, nstring::str>::iterator it = cit->second->users.find(nick);
		if (it == cit->second->users.end())
		{
			instance->log(ERROR, "handleQuit(): trying to find user in channel map on part, can't find user!");
			continue;
		}
		// can't find user in channel->users. major issue?
		
		cit->second->users.erase(it);
		if (cit->second->users.empty())
		{
			delete cit->second;
			chans.erase(cit);
		}
		// again, we just delete the channel if this is the last user in it.
	}
	// remove the user from all channel users' maps. confusing yeah.
	
	user->qChans.clear();
	// just clear it, might aswell.
}

/**
 ChannelManager::handleNick

 handle NICK changes, etc.
*/
void ChannelManager::handleNick(nstring::str &uid, nstring::str &nick)
{
	std::map<nstring::str, nstring::str>::iterator it;
	Channel* channel;
	User* user = instance->userManager->getUserFromId(uid);
	// get the user struct, we get this and search qChans, so we know what
	// channels they are in, we then find the channels and update their
	// records to tell them that nick has changed.
	
	nstring::str unick = nick;
	std::transform(unick.begin(), unick.end(), unick.begin(), ::tolower);
	// to lower chan.
	
	for (std::vector<nstring::str>::iterator i = user->qChans.begin(); i != user->qChans.end(); ++i)
	{
		channel = getChannel(*i);
		it = channel->users.find(user->oldNick);
		if (it != channel->users.end())
		{
			channel->users.insert(std::pair<nstring::str, nstring::str>(unick, it->second));
			channel->users.erase(it);
		}
		// the user SHOULD exist, but we check it anyway
	}
	// loop through qChans
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
