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
// $Id: ircdcommand.cpp 701 2009-01-31 18:31:57Z ankit $
//===================================================//

#include "ircdcommand.h"

IRCdCommand::IRCdCommand(const String cmd) : name(cmd)
{ }

IRCdCommand::~IRCdCommand()
{ }

