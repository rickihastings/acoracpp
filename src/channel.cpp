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

#include "channel.h"

/**
 Channel::Channel

 constructor
*/
Channel::Channel(nstring::str &c, std::time_t &ts) :
	name(c),
	time(ts)
{}

/**
 Channel::~Channe;

 destructor
*/
Channel::~Channel() {}
