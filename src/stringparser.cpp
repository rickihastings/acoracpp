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
// $Id: stringparser.cpp 685 2009-01-28 22:47:17Z ankit $
//===================================================//

#include "stringparser.h"


StringParser::StringParser(const String &s)
{
	Reset(s);
}


void StringParser::token(String &s, char &sep, char &sepall, char &front)
{
	if (front && *pos == front)
		++pos;

	String::iterator last;
	
	for (last = pos; pos != buf.end(); ++pos)
	{
		if (sep && *pos == sep)
		{
			s = String(last, pos);
			
			while (*pos == sep)
				++pos;
			
			return;
		}
		else if (sepall && *pos == sepall)
		{
			if (pos != last)
				s = String(last, pos);
			else
			{			
				pos = buf.end();
				s = String(last + 1, pos);
			}
			
			return;
		}
	}
	
	s = String(last, pos);
}


bool StringParser::GetToken(String &s, char sep, char sepall, char front)
{
	if (pos == buf.end())
		return false;

	while (pos != buf.end())
	{
		token(s, sep, sepall, front);
		
		if (!s.empty())
			return true;
	}
	
	return false;
}


bool StringParser::SkipToken(char sep, char sepall, char front)
{
	String s;
	return GetToken(s, sep, sepall, front);
}


bool StringParser::GetRemaining(String &s, char front)
{
	if (pos == buf.end())
		return false;
	else if (front && *pos == front)
		++pos;
	
	s = String(pos, buf.end());
	pos = buf.end();
	return !s.empty();
}


void StringParser::Reset(String s)
{
	if (!s.empty())
		buf = s;
	
	pos = buf.begin();
}


void StringParser::GetString(String &s)
{
	s = buf;
}

