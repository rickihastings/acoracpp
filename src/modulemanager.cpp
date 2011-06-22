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

#include "modulemanager.h"
#include "instance.h"
#include "socketengine.h"
#include "ircdprotocol.h"
#include "diskmanager.h"
#include "module.h"
#include "config.h"
#include "configreader.h"
#include "exception.h"

#include <dlfcn.h>
#include <iostream>

template<typename T>
T* ModuleManager::getObject(ErrorCode &error, std::string &path, const std::string func)
{
	dlerror();
	void* handle;
	T* (*getobject)(void*);

	path += ".so";
	handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);

	if (!handle)
	{
		error = err::modulemanager::noHandle;
		instance->error = nstring::str() + "dlopen() error: " + dlerror();
		return NULL;
	}
	
	*(static_cast<void**>(reinterpret_cast<void*>(&getobject))) = dlsym(handle, func.c_str());

	if (dlerror())
	{
		dlclose(handle);
		instance->error = "Module does not have a get function";
		error = err::modulemanager::noGet;
		return NULL;
	}

	T* object = NULL;
	
	try
	{
		object = (*getobject)(handle);
	}
	
	catch (Exception &e)
	{
		if (object)
			delete object;

		dlclose(handle);
		instance->error = e.getReason();
		error = err::modulemanager::exception;
		return NULL;
	}
	
	catch (std::exception &e)
	{
		if (object)
			delete object;

		dlclose(handle);
		instance->error = nstring::str() + "Module threw a standard exception: " + e.what();
		error = err::modulemanager::exception;
		return NULL;
	}
	
	if (!object)
	{
		dlclose(handle);
		instance->error = "Module returned a NULL object";
		error = err::modulemanager::nullObject;
	}
	
	return object;
}


template<typename T>
ErrorCode ModuleManager::destroyObject(T* module, void* handle)
{
	if (!handle)
		return err::modulemanager::noHandle;

	if (module)
		delete module;

	dlclose(handle);
	return err::modulemanager::none;
}


ModuleManager::ModuleManager()
{ }


ModuleManager::~ModuleManager()
{
	unloadSocketEngine();
	unloadIRCdProtocol();

	for (std::map<std::string, Module*>::iterator i = moduleList.begin(); i != moduleList.end();)
		unloadModule(i, true);

	unloadDiskManager();
}


ErrorCode ModuleManager::unloadSocketEngine()
{
	if (!instance->socketEngine)
		return err::modulemanager::notLoaded;

	destroyObject<SocketEngine>(instance->socketEngine, const_cast<void*>(instance->socketEngine->handle));	
	instance->socketEngine = NULL;
	
	instance->log(DEBUG, "ModuleManager(): Unloaded SocketEngine.");
	return err::modulemanager::none;
}


ErrorCode ModuleManager::unloadIRCdProtocol()
{
	if (!instance->ircdProtocol)
		return err::modulemanager::notLoaded;

	destroyObject<IRCdProtocol>(instance->ircdProtocol, const_cast<void*>(instance->ircdProtocol->handle));	
	instance->ircdProtocol = NULL;

	instance->log(DEBUG, "ModuleManager(): Unloaded IRCdProtocol.");
	return err::modulemanager::none;
}


ErrorCode ModuleManager::loadSocketEngine(nstring::str name)
{
	instance->log(DEBUG, "ModuleManager(): Loading SocketEngine " + name + " ...");
	
	if (instance->socketEngine)
	{
		instance->log(DEBUG, "ModuleManager(): A SocketEngine is already loaded.");
		return err::modulemanager::alreadyLoaded;
	}

	ErrorCode error;
	
	std::string path(config::SOCKDIR);
	path += name.c_str();

	if (!(instance->socketEngine = getObject<SocketEngine>(error, path, "getSocketEngine")))
		return error;

	instance->log(DEBUG, "ModuleManager(): " + name + " loaded successfully.");
	return err::modulemanager::none;
}


ErrorCode ModuleManager::loadIRCdProtocol(nstring::str name)
{
	instance->log(DEBUG, "ModuleManager(): Loading IRCdProtocol " + name + " ...");

	if (instance->ircdProtocol)
	{
		instance->log(DEBUG, "ModuleManager(): An IRCdProtocol is already loaded.");
		return err::modulemanager::alreadyLoaded;
	}

	ErrorCode error;

	std::string path(config::IRCDDIR);
	path += name.c_str();

	if (!(instance->ircdProtocol = getObject<IRCdProtocol>(error, path, "getIRCdProtocol")))
		return error;
	
	instance->log(DEBUG, "ModuleManager(): " + name + " loaded successfully.");
	return err::modulemanager::none;
}


ErrorCode ModuleManager::loadDiskManager(nstring::str &name, bool force)
{
	instance->log(DEBUG, "ModuleManager(): Loading DiskManager " + name + " ...");

	if (instance->diskManager)
	{
		if (force)
		{
			instance->log(DEBUG, "ModuleManager(): Unloading loaded DiskManager ...");
			unloadDiskManager();
		}
		else
		{
			instance->log(DEBUG, "ModuleManager(): A DiskManager is already loaded.");
			return err::modulemanager::alreadyLoaded;
		}
	}

	ErrorCode error;

	std::string path(config::DISKDIR);
	path += name.c_str();
	
	if (!(instance->diskManager = getObject<DiskManager>(error, path, "getDiskManager")))
		return error;
	
	instance->log(DEBUG, "ModuleManager(): " + name + " loaded successfully.");
	return err::modulemanager::none;
}


ErrorCode ModuleManager::unloadDiskManager()
{
	if (!instance->diskManager)
		return err::modulemanager::notLoaded;

	destroyObject<DiskManager>(instance->diskManager, const_cast<void*>(instance->diskManager->handle));	
	instance->diskManager = NULL;

	instance->log(DEBUG, "ModuleManager(): Unloaded DiskManager.");
	return err::modulemanager::none;
}


ErrorCode ModuleManager::loadModule(nstring::str &name)
{
	if (name.find(".so") != std::string::npos)
	{
		for (short i = 0; i < 3; ++i)
			name.erase(name.end() - 1);
	}

	instance->log(DEBUG, "ModuleManager(): Loading module " + name + " ...");
	
	std::map<std::string, Module*>::iterator mod = this->moduleList.find(name.c_str());
	if (mod != this->moduleList.end())
	{
		instance->log(DEBUG, "ModuleManager(): " + name + " is already loaded.");
		return err::modulemanager::alreadyLoaded;
	}

	ErrorCode error;

	std::string path(config::MODDIR);
	path += name.c_str();

	Module* m = getObject<Module>(error, path, "getModule");

	if (!m)
	{
		instance->log(ERROR, "ModuleManager(): " + instance->error);
		return error;
	}
	else
		this->moduleList[name.c_str()] = m;
	
	instance->log(DEBUG, "ModuleManager(): " + name + " loaded successfully.");
	return err::modulemanager::none;
}


ErrorCode ModuleManager::unloadModule(std::map<std::string, Module*>::iterator &mod, bool force)
{
	nstring::str name = mod->first.c_str();
	instance->log(DEBUG, "Unloading " + name + " ...");
	
	if (!force && mod->second->flags[flags::module::perm])
	{
		instance->log(DEBUG, "ModuleManager(): " + name + " is flagged permanent.");
		return err::modulemanager::perm;
	}
	
	Module* m = mod->second;
	this->moduleList.erase(mod++);
	
	destroyObject<Module>(m, const_cast<void*>(m->handle));

	instance->log(DEBUG, "ModuleManager(): " + name + " unloaded.");
	return err::modulemanager::none;
}

ErrorCode ModuleManager::reloadModule(std::map<std::string, Module*>::iterator &mod, bool force)
{
	nstring::str name = mod->first.c_str();
	instance->log(DEBUG, "ModuleManager(): Reloading " + name + " ...");
	
	if (!force && mod->second->flags[flags::module::perm])
	{
		instance->log(DEBUG, name + " is flagged permanent.");
		return err::modulemanager::perm;
	}
	
	std::string path(config::MODDIR + mod->first);

	ErrorCode error;
	Module* m = mod->second;
	Module* newm = getObject<Module>(error, path, "getModule");

	if (!newm)
	{
		instance->log(ERROR, "ModuleManager(): " + instance->error);
		return error;
	}
	else
		this->moduleList[name.c_str()] = newm;

	destroyObject<Module>(m, const_cast<void*>(m->handle));

	instance->log(DEBUG, "ModuleManager(): " + name + " reloaded.");
	return err::modulemanager::none;
}


Module* ModuleManager::getModule(nstring::str name)
{
	std::map<std::string, Module*>::iterator i = moduleList.find(name.c_str());
	
	if (i == moduleList.end())
		return NULL;
	else
		return i->second;
}


void ModuleManager::loadAutoloadModules(std::deque<nstring::str> &mods)
{
	instance->configReader->getValues(mods, "modules", "autoload");
	
	for (std::deque<nstring::str>::iterator i = mods.begin(); i != mods.end();)
	{
		if (loadModule(*i) == err::modulemanager::none)
			i = mods.erase(i);
		else
			++i;
	}
}
