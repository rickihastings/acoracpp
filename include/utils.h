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
// $Id: utils.h 690 2009-01-29 06:39:56Z ankit $
//===================================================//

#ifndef	UTILS_H
#define	UTILS_H

#include "nstring.h"

#include <sstream>

namespace utils
{
	// convert integer to string
	template<typename I> void toString(String&, I&);
	template<typename I> String toString(I);

	// convert string to integer
	template<typename I> void toInt(I&, String&);
	template<typename I> I toInt(String);

} // namespace utils

// implementation of the templates

template<typename I>
void utils::toString(String &s, I &i)
{
	std::stringstream stream;
	stream <<i;
	s = stream.str().c_str();
}

template<typename I>
String utils::toString(I i)
{
	String s;
	toString<I>(s, i);
	return s;
}

template<typename I>
void utils::toInt(I &i, String &s)
{
	std::istringstream stream(s.c_str());
	stream>> i;
}

template<typename I>
I utils::toInt(String s)
{
	I i;
	toInt<I>(i, s);
	return i;
}

#endif // UTILS_H

