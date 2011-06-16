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
// $Id: stringparser.h 685 2009-01-28 22:47:17Z ankit $
//===================================================//

#ifndef	STRINGPARSER_H
#define	STRINGPARSER_H

#include "nstring.h"
#include "utils.h"

// acora string parser
class StringParser
{
	// buffer
	String buf;
	// current iterator position
	String::iterator pos;
	
	// get next token
	void token(String&, char&, char&, char&);

public:
	// constructor
	StringParser(const String&);

	// calls token() to get next token
	bool GetToken(String&, char sep = ' ', char sepall = 0, char front = 0);
	// skip a token
	bool SkipToken(char sep = ' ', char sepall = 0, char front = 0);
	// get the remaining string
	bool GetRemaining(String&, char front = 0);
	
	// reset parser, optionally with a new string
	void Reset(String s = String());
	// get the entire buffer
	void GetString(String&);

	// get an integer token
	template<typename T>
	bool GetIntToken(T&, char sep = ' ', char sepall = 0, char front = 0);
};

// implementation of the GetIntToken template
template<typename T>
bool StringParser::GetIntToken(T &i, char sep, char sepall, char front)
{
	String s;
	
	if (!GetToken(s, sep, sepall, front))
		return false;
	else
	{
		utils::toInt<T>(i, s);
		return true;
	}
}

#endif // STRINGPARSER_H

