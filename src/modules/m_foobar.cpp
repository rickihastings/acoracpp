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

#include "module.h"

#include <iostream>

// the module class
class ModuleFoobar : public Module
{
	public:
	
	// constructor
	ModuleFoobar(void* h)
	: Module(h, "m_foobar.cpp", "General")
	{
		
	}
	
	virtual void onJoin(int t1, int t2)
	{
		std::cout << "FUCKIN TEST MAN " << t1 << " " << t2 << std::endl;
	}
};

// initialize the module
INIT_MODULE(ModuleFoobar)

