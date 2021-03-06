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
#include "modeparser.h"
#include "ircdprotocol.h"
#include "utils.h"
#include "modulemanager.h"
#include "module.h"

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
	irc::user_hash::iterator i;
	for (i = users.begin(); i != users.end(); ++i)
		delete i->second;
	users.clear();
	uidMap.clear();
}

/**
 UserManager::handleConnect

 handle incoming users
*/
void UserManager::handleConnect(nstring::str &nick, nstring::str &ident, nstring::str &host, nstring::str &ip, nstring::str &nickTS, nstring::str &uid, nstring::str &sid, nstring::str &gecos)
{
	users.insert(std::pair<nstring::str, User*>(nick, new User(uid, nick, ident, host, ip, gecos, sid, nickTS)));
	uidMap.insert(std::pair<nstring::str, nstring::str>(uid, nick));
	// insert user into uid map so we can find it easily
	
	nstring::str server = instance->ircdProtocol->getServerFromId(sid);
	instance->log(MISC, "handleConnect(): " + nick + "!" + ident + "@" + host + " (" + uid + ") has connected to " + server);
	//instance->log(LOGCHAN, "// TODO");
	// log things, ie LOGCHAN and NETWORK
	
	FOREACH_MODULE(instance, &Module::onConnect, nick, uid, ident, host, ip);
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
	
	irc::nhash::iterator i = uidMap.find(uid);
	irc::user_hash::iterator it = users.find(nick);
	if (i != uidMap.end() && it != users.end())
	{
		nstring::str server = instance->ircdProtocol->getServerFromId(it->second->server);
		instance->log(MISC, "handleQuit(): " + it->second->nick + "!" + it->second->ident + "@" + it->second->host + " has disconnected from " + server);
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
	
	FOREACH_MODULE(instance, &Module::onQuit, nick);
}

/**
 UserManager::handleNick

 handle nick, renames our nick in users and uidMap
*/
void UserManager::handleNick(nstring::str &uid, nstring::str &nick)
{
	User* userHolder = NULL;
	nstring::str userNick;
	getNickFromId(uid, userNick);
	// get the class from users
	
	irc::user_hash::iterator it = users.find(userNick);
	irc::nhash::iterator i = uidMap.find(uid);
	// find uid maps
	
	instance->log(MISC, "handleNick(): " + it->second->nick + "!" + it->second->ident + "@" + it->second->host + " changed nick to " + nick);
	//instance->log(LOGCHAN, "// TODO");
	// log things, ie LOGCHAN and NETWORK
	
	userHolder = it->second;
	i->second = nick;
		
	userHolder->oldNick = userHolder->nick;
	userHolder->nick = nick;
		
	users.insert(std::pair<nstring::str, User*>(nick, userHolder));
	users.erase(it);
	// update some info
	
	FOREACH_MODULE(instance, &Module::onNick, userNick, nick);
}

/**
 UserManager::handleMode

 handle mode changes
*/
void UserManager::handleMode(nstring::str &uid, nstring::str &modes)
{
	irc::modes modeContainer;
	irc::params paramContainer;
	instance->modeParser->sortModes(modes, modeContainer, paramContainer, false);
	// parse modes

	nstring::str nick;
	getNickFromId(uid, nick);
	User* user = getUser(nick);
	instance->modeParser->saveModes(user, modeContainer);
	// update the modes in our user record

	if (modeContainer["plus"].find('o') != std::string::npos)
		handleOper(uid, true);
	// check if we have an "+o", if we do call handleOper()
	if (modeContainer["minus"].find('o') != std::string::npos)
		handleOper(uid, false);
	// check if we have a "-o", if we do again call handleOper()
	
	FOREACH_MODULE(instance, &Module::onUMode, nick, modeContainer);
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
	
	irc::user_hash::iterator it = users.find(userNick);
	// find uid maps
	
	instance->log(MISC, "handleHost(): " + it->second->nick + "!" + it->second->ident + "@" + it->second->host + " changed hostname to " + host);
	//instance->log(LOGCHAN, "// TODO");
	// log things, ie LOGCHAN and NETWORK
	
	it->second->oldHost = it->second->host;
	it->second->host = host;
	// update host name
	
	FOREACH_MODULE(instance, &Module::onHostChange, userNick, host);
}

/**
 UserManager::handleOper

 handle oper ups (mode = true/false)
*/
void UserManager::handleOper(nstring::str &uid, bool mode)
{
	nstring::str nick;
	getNickFromId(uid, nick);
	User* userHolder = getUser(nick);
	userHolder->oper = mode;
	// set oper to mode
	
	//instance->log(MISC, "handleConnect(): " + nick + "!" + ident + "@" + host + " has connected to " + sid);
	//instance->log(LOGCHAN, "// TODO");
	// log things, ie LOGCHAN and NETWORK
	
	FOREACH_MODULE(instance, &Module::onOper, nick, mode);
}

/**
 UserManager::getNickFromId

 get nickname from id
*/
void UserManager::getNickFromId(nstring::str &uid, nstring::str &nick)
{
	irc::nhash::iterator it = uidMap.find(uid);
	
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
	irc::user_hash::iterator it = users.find(nick);
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
