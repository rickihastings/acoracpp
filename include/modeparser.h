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

#ifndef MODE_H
#define MODE_H

#include "base.h"

class User;
class Channel;

class ModeParser
{
public:

    // check modes
    void checkModes(nstring::str&);
    // sort modes
    void sortModes(nstring::str&, irc::modes&, irc::params&, bool);
    // save modes
    void saveModes(Channel*, irc::modes&, irc::params&);
	// save modes
	void saveModes(User*, irc::modes&);
	
	// constructor
	ModeParser();
	// destructor
	virtual ~ModeParser();
};

#endif // MODE_H