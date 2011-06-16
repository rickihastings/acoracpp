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
// $Id: nstring.cpp 689 2009-01-29 06:33:19Z ankit $
//===================================================//

#include "nstring.h"
#include "charmaps.h"


bool nstring::char_traits::eq(char c1, char c2)
{
	return (charmaps::lower[static_cast<short>(c1)] == charmaps::lower[static_cast<int>(c2)]);
}


bool nstring::char_traits::ne(char c1, char c2)
{
	return (charmaps::lower[static_cast<short>(c1)] != charmaps::lower[static_cast<short>(c2)]);
}


bool nstring::char_traits::lt(char c1, char c2)
{
	return (charmaps::lower[static_cast<short>(c1)] < charmaps::lower[static_cast<short>(c2)]);
}


int nstring::char_traits::compare(const char* str1, const char* str2, size_t n)
{
	for (std::size_t i = 0; i < n; ++i)
	{
		if (!str1[i] || !str2[i])
		{
			if (str1[i])
				return 1;
			else if (str2[i])
				return -1;
			else
				return 0;
		}

		if (lt(str1[i], str2[i]))
			return -1;
		else if (lt(str2[i], str1[i]))
			return 1;
	}

	return 0;
}
