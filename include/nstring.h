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
// $Id: nstring.h 685 2009-01-28 22:47:17Z ankit $
//===================================================//

#ifndef	NSTRING_H
#define	NSTRING_H

#include <string>

namespace nstring
{
	// our char_traits for nstring::str, allows case insensitive comparisions of strings
	struct char_traits : public std::char_traits<char>
	{
		// true if two char's are equal
		static bool eq(char, char);
		// true if two char's are not equal
		static bool ne(char, char);
		// true if first char is lower than second char
		static bool lt(char, char);
		// compare two strings
		static int compare(const char*, const char*, size_t);
	};
	
	// implements acora_char_traits
	typedef std::basic_string<char, nstring::char_traits, std::allocator<char> > str;
} // namespace nstring

#endif // NSTRING_H

