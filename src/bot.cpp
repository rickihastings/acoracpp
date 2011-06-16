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
// $Id: bot.cpp 709 2009-02-01 12:33:20Z ankit $
//===================================================//

#include "bot.h"
#include "service.h"

Bot::Bot()
{ }

Bot::~Bot()
{
	for (std::deque<Service*>::iterator i = services.begin(); i != services.end(); ++i)
		delete *i;
}

