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

#include "diskmanager.h"

// the default disk manager
class DiskDefault : public DiskManager
{
public:
	// constructor
	DiskDefault(void* h)
	: DiskManager(h, "disk_default.cpp 0.0.1 acora")
	{ }
};

// initialize the disk manager
initDiskManager(DiskDefault)

