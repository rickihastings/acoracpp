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
// $Id: exception.h 702 2009-01-31 18:50:56Z ankit $
//===================================================//

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "nstring.h"

#include <exception>

// exceptions for acora
class Exception : public std::exception
{
protected:
	// source of the error
	const String src;
	// the error
	const String err;

public:
	// constructor
	Exception(const String src, const String err);
	// destructor
	virtual ~Exception() throw ();
	
	// get reason for the exception
	virtual const char* getReason();
	// get source
	virtual const char* getSource();
};

#endif // EXCEPTION_H

