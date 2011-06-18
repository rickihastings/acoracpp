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
// $Id: socketengine.h 708 2009-02-01 11:48:00Z ankit $
//===================================================//

#ifndef	SOCKETENGINE_H
#define	SOCKETENGINE_H

#include "base.h"

// errors for socketengine
namespace err
{
	namespace socketengine
	{
		enum
		{
			none,			// no error
			empty,			// no buffer received
			engine,			// error in socket engine
			nobuf,			// no buffer
			socket,			// error in socket
			recv,			// error in recv()
			send,			// error in send()
			closed,			// connection was closed
			shutdown,		// error in shutdown()
			close,			// error in close()
			notConnected,		// not connected
			alreadyConnected,	// already connected
			resolve,		// could not resolve address
			local,			// error in local address
			remote			// error in remote address
		};
	}
}

// base class for socket engines, these modules provide
// a way of managing sockets and connections with the
// remote server
class SocketEngine
{
protected:
	// true if connection is established
	bool connected;

public:
	// module handle
	const void* handle;
	// module version
	const nstring::str version;

	// constructor
	SocketEngine(void*, const nstring::str version);
	// destructor
	virtual ~SocketEngine();

	// true if connection is established
	virtual bool isConnected();

	// connect to remote server
	virtual ErrorCode connect() = 0;
	// send data
	virtual ErrorCode send(nstring::str&) = 0;
	// receive data
	virtual ErrorCode recv(nstring::str&) = 0;
	// disconnect
	virtual ErrorCode disconnect() = 0;

	// send string
	virtual ErrorCode sendString(nstring::str);
};

// macro to initialize a socket engine
#define initSocketEngine(class_name) \
	extern "C" SocketEngine* getSocketEngine(void* handle) \
	{ \
		return new class_name(handle); \
	}

#endif // SOCKETENGINE_H

