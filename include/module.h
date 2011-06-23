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

#ifndef	MODULE_H
#define	MODULE_H

#include "base.h"

#include <bitset>
#include <boost/thread.hpp>

// flags
namespace flags
{
	namespace module
	{
		enum
		{
			perm	// module is permanent
		};
	}
}

class ModuleManager;

// base class for modules
class Module
{
public:
	// module handle
	const void* handle;

	// module version
	const nstring::str version;
	// module type
	const nstring::str type;
	// module flags
	const std::bitset<1> flags;

	// constructor
	Module(void* handle, const nstring::str version, const nstring::str type, unsigned long flags = 0);
	// destructor
	virtual ~Module();
	
	// on create
	virtual void onChanCreate(nstring::str&, std::map<nstring::str, nstring::str>&);
	// on join
	virtual void onJoin(nstring::str&, nstring::str&);
	// on part
	virtual void onPart(nstring::str&, nstring::str&);
	// on kick
	
	// on mode
	virtual void onCMode(nstring::str&, irc::modes&, irc::params&);
	// on topic
	virtual void onTopic(nstring::str&, nstring::str&, nstring::str&);
	// on connect
	virtual void onConnect(nstring::str&, nstring::str&, nstring::str&, nstring::str&, nstring::str&);
	// on quit
	virtual void onQuit(nstring::str&);
	// on nick
	virtual void onNick(nstring::str&, nstring::str&);
	// on umode
	virtual void onUMode(nstring::str&, irc::modes&);
	// on host change
	virtual void onHostChange(nstring::str&, nstring::str&);
	// on oper
	virtual void onOper(nstring::str&, bool&);
};

/**
 FOREACH_MODULE

 macro to call a method in all modules
*/
#define FOREACH_MODULE(x, y, z...) \
    for (std::map<std::string, Module*>::iterator mit = x->moduleManager->moduleList.begin(); mit != x->moduleManager->moduleList.end(); ++mit) \
    { \
		boost::thread newThread(y, mit->second, z); \
	}

/**
 INIT_MODULE

 macro to initialize a module
*/
#define INIT_MODULE(class_name) \
	extern "C" Module* getModule(void* handle) \
	{ \
		return new class_name(handle); \
	}

#endif // MODULE_H

