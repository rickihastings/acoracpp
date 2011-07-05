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
	
	// case insensitive string hash compare
	struct ci_less : std::binary_function<nstring::str, nstring::str, bool>
	{
		// case-independent (ci) compare_less binary function
		struct nocase_compare : public std::binary_function<unsigned char, unsigned char, bool> 
		{
			bool operator() (const unsigned char& c1, const unsigned char& c2) const {
				return tolower (c1) < tolower (c2); 
			}
		};
		bool operator() (const nstring::str &s1, const nstring::str &s2) const {
			return std::lexicographical_compare 
			(s1.begin(), s1.end(),   // source range
			s2.begin(), s2.end(),   // dest range
			nocase_compare());  // comparison
		}
	};
} // namespace nstring

#endif // NSTRING_H

