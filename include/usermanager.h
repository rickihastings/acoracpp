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
// $Id: usermanager.h 706 2009-01-31 20:17:11Z ankit $
//===================================================//

#ifndef	USERMANAGER_H
#define	USERMANAGER_H

#include "extensible.h"

// manages users on the network
class UserManager : public Extensible
{
public:
	// constructor
	UserManager();
	// destructor
	virtual ~UserManager();
};

#endif // USERMANAGER_H

