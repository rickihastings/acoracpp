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
// $Id: service.h 709 2009-02-01 12:33:20Z ankit $
//===================================================//

#ifndef	SERVICE_H
#define	SERVICE_H

#include "extensible.h"

// a service is the brain of a Bot, it defines how
// a message from a user in processed for the service
// and how it responds to it using the Bot that
// contains it. a bot can have more than one services
class Service : public Extensible
{
public:
	// constructor
	Service();
	// destructor
	virtual ~Service();
};

#endif // SERVICE_H

