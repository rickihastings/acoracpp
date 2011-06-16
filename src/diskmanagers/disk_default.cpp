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
// $Id: disk_default.cpp 688 2009-01-29 00:05:25Z ankit $
//===================================================//

#include "diskmanager.h"

// the default disk manager
class DiskDefault : public DiskManager
{
public:
	// constructor
	DiskDefault(void* h)
	: DiskManager(h, "$Id: disk_default.cpp 688 2009-01-29 00:05:25Z ankit $")
	{ }
};

// initialize the disk manager
initDiskManager(DiskDefault)

