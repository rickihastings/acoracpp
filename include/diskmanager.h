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
// $Id: diskmanager.h 708 2009-02-01 11:48:00Z ankit $
//===================================================//

#ifndef	DISKMANAGER_H
#define	DISKMANAGER_H

#include "extensible.h"

// base class for disk managers, these modules provide
// a way of writing and reading databases to and from
// disk
class DiskManager : public Extensible
{
public:
	// module handle
	const void* handle;
	// module version
	const String version;

	// constructor
	DiskManager(void*, const String version);
	// destructor
	virtual ~DiskManager();
};

// macro to initialize a disk manager
#define initDiskManager(class_name) \
	extern "C" DiskManager* getDiskManager(void* handle) \
	{ \
		return new class_name(handle); \
	}

#endif // DISKMANAGER_H

