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
#include <boost/bind.hpp>

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
	
	// join
	virtual void onJoin(int, int);
};

/**
 FOREACH_MODULE

 macro to call a method in all modules
*/
#define FOREACH_MODULE(x, y, z...) \
    for (std::map<std::string, Module*>::iterator mit = x->moduleManager->moduleList.begin(); mit != x->moduleManager->moduleList.end(); ++mit) \
    { \
		boost::thread newThread(boost::bind(y, mit->second, z)); \
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

