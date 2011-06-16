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
// $Id: extensible.h 697 2009-01-29 16:08:40Z ankit $
//===================================================//

#ifndef	EXTENSIBLE_H
#define	EXTENSIBLE_H

#include "base.h"
#include "nstring.h"

#include <map>
#include <deque>

namespace extensible
{
	// the metadata class
	class Metadata
	{
	public:
		// objects
		std::map<std::string, void*> objects;
		// object lists
		std::map<std::string, std::deque<void*> > objectList;
		// strings
		std::map<std::string, String> strings;
		// string lists
		std::map<std::string, std::deque<String> > stringList;
	};
} // namespace extensible

// classes inheriting this can extend themselves
// by adding metadata to the above mentioned maps
class Extensible : public Base
{
public:
	// metadata for the object is saved here
	extensible::Metadata* metadata;

	// constructor
	Extensible();
	// destructor
	virtual ~Extensible();
};

#endif // EXTENSIBLE_H

