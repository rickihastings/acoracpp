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
// $Id: diskmanager.cpp 698 2009-01-29 16:33:02Z ankit $
//===================================================//

#include "diskmanager.h"

DiskManager::DiskManager(void* h, const nstring::str ver) :
	handle(h),
	version(ver)
{ }

DiskManager::~DiskManager()
{ }

