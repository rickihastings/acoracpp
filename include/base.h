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
// $Id: base.h 697 2009-01-29 16:08:40Z ankit $
//===================================================//

#ifndef	BASE_H
#define	BASE_H

#include "nstring.h"

#include <map>
#include <vector>
#include <ctime>
#include <stdarg.h>
#include <stdio.h>

// the base class for all classes
class Base
{
public:
	// constructor
	Base();
	// destructor
	virtual ~Base();
};

typedef short ErrorCode;

#endif // BASE_H

