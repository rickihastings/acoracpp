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

#include <boost/thread.hpp>
#include <tr1/unordered_map>

#define hash_map unordered_map
#define nspace std::tr1

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
class User;
class Channel;

/**
 irc

 our irc typedefs namespace
*/
namespace irc
{
	/**
     typedefs
     */
    typedef std::map<nstring::str, nstring::str> modes;
    typedef std::map<nstring::str, std::map<nstring::str, nstring::str> > params;
	typedef std::map<nstring::str, nstring::str, std::less<nstring::str> > nhash;
	typedef std::map<nstring::str, User*, std::less<nstring::str> > user_hash;
	typedef std::map<nstring::str, Channel*, std::less<nstring::str> > chan_hash;
}

#endif // BASE_H

