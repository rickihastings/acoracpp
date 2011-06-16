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
// $Id: modulemanager.h 708 2009-02-01 11:48:00Z ankit $
//===================================================//

#ifndef	MODULEMANAGER_H
#define	MODULEMANAGER_H

#include "extensible.h"

#include <map>
#include <deque>

// modulemanager errors
namespace err
{
	namespace modulemanager
	{
		enum
		{
			none,		// no error
			exception,	// module threw an exception
			alreadyLoaded,	// module is already loaded
			noHandle,	// no module handle
			noGet,		// no get function
			nullObject,	// null object returned by module
			notLoaded,	// module not loaded
			noDestroy,	// no destruction function
			perm		// module is permanent
		};
	}
}

class Module;

// manages all kinds of modules
class ModuleManager : public Extensible
{
protected:
	// get module object
	template<typename T>
	T* getObject(ErrorCode &error, std::string &path, const std::string func);
	// destroy module object
	template<typename T>
	ErrorCode destroyObject(T* module, void* handle);

	// unload the socket engine
	virtual ErrorCode unloadSocketEngine();
	// unload the ircd protocol
	virtual ErrorCode unloadIRCdProtocol();

public:
	// module list
	std::map<std::string, Module*> moduleList;

	// constructor
	ModuleManager();
	// destructor
	virtual ~ModuleManager();

	// load the socket engine
	virtual ErrorCode loadSocketEngine(String);
	// load the ircd protocol
	virtual ErrorCode loadIRCdProtocol(String);

	// load the disk manager
	virtual ErrorCode loadDiskManager(String&, bool = false);
	// unload the disk manager
	virtual ErrorCode unloadDiskManager();

	// load a module
	virtual ErrorCode loadModule(String&);
	// unload a module
	virtual ErrorCode unloadModule(std::map<std::string, Module*>::iterator&, bool = false);
	// reload a module
	virtual ErrorCode reloadModule(std::map<std::string, Module*>::iterator&, bool = false);

	// return a Module*
	virtual Module* getModule(String);

	// load autoload modules specified in the
	// config file
	virtual void loadAutoloadModules(std::deque<String> &list);
};

#endif // MODULEMANAGER_H

