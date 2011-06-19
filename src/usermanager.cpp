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
#include "usermanager.h"
#include "utils.h"

#include <algorithm>
#include <iostream>

/**
 UserManager::UserManager

 constructor
*/
UserManager::UserManager()
{ }

/**
 UserManager::~UserManager

 destructor
*/
UserManager::~UserManager()
{
	std::map<nstring::str, User*>::iterator i;
	for (i = users.begin(); i != users.end(); ++i)
		delete i->second;
	users.clear();
	uidMap.clear();
}

/**
 UserManager::handleConnect

 handle incoming users
*/
void UserManager::handleConnect(nstring::str &nick, nstring::str &ident, nstring::str &host, nstring::str &realhost, nstring::str &ip, nstring::str &accountName, nstring::str &nickTS, nstring::str &uid, nstring::str &modes, nstring::str &sid, nstring::str &gecos)
{
	std::time_t timeStamp;
	std::istringstream stream(nickTS.c_str());
	stream >> timeStamp;
	// get the user timestamp
	
	nstring::str unick = nick;
	std::transform(unick.begin(), unick.end(), unick.begin(), ::tolower);
	
	users.insert(std::pair<nstring::str, User*>(unick, new User(uid, nick, ident, host, realhost, ip, modes, gecos, sid, timeStamp)));
	uidMap.insert(std::pair<nstring::str, nstring::str>(uid, unick));
	// insert user into uid map so we can find it easily
	
	instance->log(NETWORK, "handleConnect(): " + nick + "!" + ident + "@" + host + " has connected to " + sid);
	//instance->log(LOGCHAN, "// TODO");
	// log things, ie LOGCHAN and NETWORK
}

/**
 UserManager::handleQuit

 handle quits (ie delete the class)
*/
void UserManager::handleQuit(nstring::str &uid)
{
	nstring::str nick;
	getNickFromId(uid, nick);
	// get nickname from uid.
	
	std::map<nstring::str, nstring::str>::iterator i = uidMap.find(uid);
	std::map<nstring::str, User*>::iterator it = users.find(nick);
	if (i != uidMap.end() && it != users.end())
	{
		instance->log(NETWORK, "handleQuit(): " + it->second->nick + "!" + it->second->ident + "@" + it->second->host + " has disconnected from " + it->second->server);
		//instance->log(LOGCHAN, "// TODO");
		// log things, ie LOGCHAN and NETWORK
	
		delete it->second;
		users.erase(it);
		uidMap.erase(i);
		// remove the user
		
		return;
		// we're done, bail!
	}
	// user deleted
	
	instance->log(ERROR, "handleQuit(): Cannot find the user in user map, this will possibly cause a leak!");
	// log a few things here
}

/**
 UserManager::handleNick

 handle nick, renames our nick in users and uidMap
*/
void UserManager::handleNick(nstring::str &uid, nstring::str &nick)
{
	static User* userHolder;
	nstring::str userNick;
	getNickFromId(uid, userNick);
	// get the class from users
	nstring::str lnick = nick;
	std::transform(lnick.begin(), lnick.end(), lnick.begin(), ::tolower);
	// tolower our nick
	
	std::map<nstring::str, User*>::iterator it = users.find(userNick);
	std::map<nstring::str, nstring::str>::iterator i = uidMap.find(uid);
	// find uid maps
	
	instance->log(NETWORK, "handleNick(): " + it->second->nick + "!" + it->second->ident + "@" + it->second->host + " changed nick to " + nick);
	//instance->log(LOGCHAN, "// TODO");
	// log things, ie LOGCHAN and NETWORK
	
	userHolder = it->second;
	i->second = lnick;
		
	userHolder->oldNick = userHolder->nick;
	userHolder->nick = nick;
		
	users.insert(std::pair<nstring::str, User*>(lnick, userHolder));
	users.erase(it);
	// update some info
}

/**
 UserManager::handleMode

 handle mode changes
*/
void UserManager::handleMode(nstring::str &uid, nstring::str &modes)
{
	
}

/**
 UserManager::handleHost

 handle host changes, typically CHGHOST
*/
void UserManager::handleHost(nstring::str &uid, nstring::str &host)
{
	nstring::str userNick;
	getNickFromId(uid, userNick);
	// get the class from users
	
	std::map<nstring::str, User*>::iterator it = users.find(userNick);
	// find uid maps
	
	instance->log(NETWORK, "handleHost(): " + it->second->nick + "!" + it->second->ident + "@" + it->second->host + " changed hostname to " + host);
	//instance->log(LOGCHAN, "// TODO");
	// log things, ie LOGCHAN and NETWORK
	
	it->second->oldHost = it->second->host;
	it->second->host = host;
	// update host name ^_^
}

/**
 UserManager::handleOper

 handle oper ups (mode = true/false)
*/
void UserManager::handleOper(nstring::str &uid, bool mode)
{
	User* userHolder = getUserFromId(uid);
	userHolder->oper = mode;
	// set oper to mode
}

/**
 UserManager::getNickFromId

 get nickname from id
*/
void UserManager::getNickFromId(nstring::str &uid, nstring::str &nick)
{
	std::map<nstring::str, nstring::str>::iterator it = uidMap.begin();
	
	if (it != uidMap.end())
		nick = it->second;
	else
		instance->log(ERROR, "getNickFromId(): Unable to find uid in uidMap, this WILL cause problems!");
	// get user from id, if we can!
}

/**
 UserManager::getUserFromId

 return User* from id
*/
User* UserManager::getUserFromId(nstring::str &uid)
{
	nstring::str nick;
	getNickFromId(uid, nick);
	// get nickname from uid.
	
	return getUser(nick);
	// make use of getUser to return User* from nick.
}

/**
 UserManager::getUser

 return User* from nick
*/
User* UserManager::getUser(nstring::str &nick)
{
	nstring::str unick = nick;
	std::transform(unick.begin(), unick.end(), unick.begin(), ::tolower);
	// to lower it, seen as though everything in users is tolower.
	
	std::map<nstring::str, User*>::iterator it = users.find(unick);
	if (it != users.end())
	{
		return it->second;
	}
	else
	{
		instance->log(ERROR, "getUser(): trying to find invalid user, this can cause serious issues!");
		return NULL;
	}
	// find the user class from our nick
}
