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
// $Id: module.h 708 2009-02-01 11:48:00Z ankit $
//===================================================//

#ifndef	MODULE_H
#define	MODULE_H

#include "base.h"

#include <bitset>

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
};

// macro to initialize a module
#define initModule(class_name) \
	extern "C" Module* getModule(void* handle) \
	{ \
		return new class_name(handle); \
	}

#endif // MODULE_H

