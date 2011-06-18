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
#include <vector>

/**
 helper

 helper functions
*/
namespace utils
{
    // to str
	template<typename T> void toStr(nstring::str&, const T &i);
    template<typename T> nstring::str toStr(T);
    // to int
    template<typename T> void toInt(T&, nstring::str&);
	template<typename T> T toInt(nstring::str);
    // to float
    template<typename T> float toFloat(const T &i);
    // filter
    template<typename T> void filter(T &str);
	// trim
	template<typename T> inline T left_trim(const T& src, const T& to_trim);
	template<typename T> inline T right_trim(const T& src, const T& to_trim);
	template<typename T> inline T trim(const T& src, const T& to_trim);
    // explode
    template<typename T> void explode(const char* delim, T &str, std::vector<T> &tokens);
    // replace
    void replace(nstring::str &s, nstring::str f, nstring::str r);
	// getDataAfter
	template<typename T> nstring::str getDataAfter(const T &data, unsigned int where);
	// stripColon
	template<typename T> void stripColon(T &data);
}

/**
 utils::toStr

 convert integer to string
*/
template<typename T>
void utils::toStr(nstring::str &s, const T &i)
{
    std::stringstream stream;
	stream << i;
	s = stream.str().c_str();
}

template<typename T>
nstring::str utils::toStr(T i)
{
    nstring::str s;
	toStr<T>(s, i);
	return s;
}

/**
 utils::toInt

 convert string to integer
*/
template<typename T>
void utils::toInt(T &i, nstring::str &s)
{
    std::istringstream stream(s.c_str());
	stream >> i;
}

template<typename T>
T utils::toInt(nstring::str s)
{
	T i;
	toInt<T>(i, s);
	return i;
}

/**
 utils::toFloat

 convert string to float
*/
template<typename T>
inline float utils::toFloat(const T &i)
{
    float number;

    std::istringstream stream(i);
    stream >> number;
    return number;
}

/**
 utils::filter

 filter
*/
template<typename T>
void utils::filter(T &str)
{
    T newStr;
    int strleng = str.length();
    int i = 0;

    while (i < strleng)
    {
        if (str[i] == '\n')
        {
            ++i;
            continue;
        }
        if (str[i] == '\r')
        {
            ++i;
            continue;
        }
        if (str[i] == '\0')
        {
            ++i;
            continue;
        }
        if (str[i] == '\t')
        {
            str[i] = ' ';
            ++i;
            continue;
        }

        newStr += str[i];
        ++i;
    }

    str = newStr;
}

/**
 utils::left_trim
*/
template<typename T>
inline T utils::left_trim(const T& src, const T& to_trim)
{
	if(!src.length())
		return src;
	return src.substr(src.find_first_not_of(to_trim));
}

/**
 utils::right_trim
*/
template<typename T>
inline T utils::right_trim(const T& src, const T& to_trim)
{
	if(!src.length())
		return src;
	return src.substr(0, src.find_last_not_of(to_trim) + 1);
}

/**
 utils::trim
*/
template<typename T>
inline T utils::trim(const T& src, const T& to_trim) { return utils::right_trim(utils::left_trim(src, to_trim), to_trim); }

/**
 utils::explode

 explode
*/
template<typename T>
void utils::explode(const char* delim, T &str, std::vector<T> &tokens)
{
    // Skip delimiters at beginning.
    nstring::str::size_type lastPos = str.find_first_not_of(delim, 0);
    // Find first "non-delimiter".
    nstring::str::size_type pos = str.find_first_of(delim, lastPos);

    while (nstring::str::npos != pos || nstring::str::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delim, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delim, lastPos);
    }
}

/**
 utils::replace

 recursive str replace
*/
inline void utils::replace(nstring::str &s, nstring::str f, nstring::str r)
{
    int find = 0;
    std::size_t rpos = 0;

    while (true)
    {
        if (find == 0)
            rpos = s.find(f);
        else
            rpos = s.find(f, rpos + (f.length() + 1));

        if (rpos != nstring::str::npos)
        {
            ++find;
            s.replace(rpos, f.length(), r);
        }
        else
            break;
    }
}

/**
 utils::getDataAfter

 get data after (nstring::str)
*/
template<typename T>
nstring::str utils::getDataAfter(const T &data, unsigned int where)
{
    nstring::str returnVal = "";
    unsigned int i;

    for (i = where; i != data.size(); ++i)
    {
        if (i != where)
            returnVal += ' ';

        returnVal.append(data.at(i));
    }

    return returnVal;
}

/**
 utils::stripColon

 if colon is first letter strip it.
*/
template<typename T>
void utils::stripColon(T &data)
{
	if (*(data.begin()) == ':')
		data.erase(data.begin());
}

#endif // UTILS_H

