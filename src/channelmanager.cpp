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
#include "channelmanager.h"
#include "usermanager.h"

#include <iostream>

ChannelManager::ChannelManager()
{ }

ChannelManager::~ChannelManager()
{ }

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
			tempNick.erase(std::remove_if(tempNick.begin(), tempNick.end(), utils::isNotAlnum), tempNick.end());
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
 ChannelManager::handleCreate

 handle SJOINS or FJOIN w/e
*/
void ChannelManager::handleCreate(nstring::str &chan, nstring::str &ts, nstring::str &modes, std::vector<nstring::str> &users)
{
	//std::map<nstring::str, nstring::str> parsedUsers = parseUsers(users);
}
