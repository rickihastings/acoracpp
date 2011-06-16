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
// $Id: exception.cpp 702 2009-01-31 18:50:56Z ankit $
//===================================================//

#include "exception.h"

Exception::Exception(const String source, const String error) :
	src(source),
	err(error)
{ }

Exception::~Exception() throw ()
{ }

const char* Exception::getReason()
{
	return err.c_str();
}

const char* Exception::getSource()
{
	return src.c_str();
}

