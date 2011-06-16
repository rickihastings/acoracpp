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
// $Id: socketengine.cpp 698 2009-01-29 16:33:02Z ankit $
//===================================================//

#include "socketengine.h"

SocketEngine::SocketEngine(void* h, const String ver) :
	connected(false),
	handle(h),
	version(ver)
{ }

SocketEngine::~SocketEngine()
{ }

ErrorCode SocketEngine::sendString(String buf)
{
	return send(buf);
}

bool SocketEngine::isConnected()
{
	return connected;
}

