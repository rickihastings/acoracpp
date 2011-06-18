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
	
	std::map<nstring::str, User*>::iterator it = users.end();
	users.insert(it, std::pair<nstring::str, User*>(unick, new User(uid, nick, ident, host, realhost, ip, modes, gecos, sid, timeStamp)));
	// insert user into array
	std::map<nstring::str, nstring::str&>::iterator i = uidMap.end();
	uidMap.insert(i, std::pair<nstring::str, nstring::str&>(uid, unick));
	// insert user into uid map so we can find it easily
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
	
	std::map<nstring::str, User*>::iterator it = users.find(nick);
	if (it != users.end())
	{
		delete it->second;
		users.erase(it);
		return;
	}
	// user deleted
	
	instance->log(ERROR, "handleQuit(): Cannot find the user in user map, this will possibly cause a leak!");
	// something has SERIOUSLY gone wrong here.
}

/**
 UserManager::getNickFromId

 get nickname from id
*/
void UserManager::getNickFromId(nstring::str &uid, nstring::str &nick)
{
	std::map<nstring::str, nstring::str&>::iterator it = uidMap.find(uid);
	nick = it->second;
	// get user from id
}

/**
 UserManager::getUser

 get user from id
*/
/*User* UserManager::getUser(nstring::str &uid)
{

}*/

