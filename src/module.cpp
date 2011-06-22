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

Module::Module(void* h, const nstring::str ver, const nstring::str t, unsigned long f) :
	handle(h),
	version(ver),
	type(t),
	flags(f)
{ }

Module::~Module()
{ }

void Module::onJoin(int, int)
{ }
