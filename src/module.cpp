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
// $Id: module.cpp 698 2009-01-29 16:33:02Z ankit $
//===================================================//

#include "module.h"

Module::Module(void* h, const String ver, const String t, unsigned long f) :
	handle(h),
	version(ver),
	type(t),
	flags(f)
{ }

Module::~Module()
{ }

