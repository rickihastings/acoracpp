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
// $Id: m_foobar.cpp 688 2009-01-29 00:05:25Z ankit $
//===================================================//

#include "module.h"

// the module class
class ModuleFoobar : public Module
{
public:
	// constructor
	ModuleFoobar(void* h)
	: Module(h, "$Id: m_foobar.cpp 688 2009-01-29 00:05:25Z ankit $", "General")
	{ }
};

// initialize the module
initModule(ModuleFoobar)

