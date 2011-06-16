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
// $Id: sock_select.cpp 692 2009-01-29 09:15:03Z ankit $
//===================================================//

#include "socketengine.h"
#include "instance.h"
#include "configreader.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cerrno>
#include <cstring>


// the select socket engine
class SelectEngine : public SocketEngine
{
	// send buffer
	ErrorCode send(const char* buf, std::size_t len)
	{
		if (!len)
			return err::socketengine::nobuf;

		short rem;

		while (len > 0)
		{
			rem = ::send(fd, buf, len, 0);

			if (rem == -1)
			{
				instance->error = String() + "send() error: " + std::strerror(errno);
				return err::socketengine::send;
			}

			len -= rem;
			buf += rem;
		}

		return err::socketengine::none;
	}
	
	
public:
	// fd used to connect to remote server
	int fd;

	// socket sets
	fd_set set;
	fd_set eset;
	
	
	// constructor
	SelectEngine(void* h)
	: SocketEngine(h, "$Id: sock_select.cpp 692 2009-01-29 09:15:03Z ankit $")
	{ }
	
	
	// destructor
	~SelectEngine()
	{
		if (isConnected())
			disconnect();
	}
	
	
	// connect to the remote server
	ErrorCode connect()
	{
		instance->debug("Connecting ...");
		
		if (isConnected())
		{
			instance->debug("Already connected.");
			return err::socketengine::alreadyConnected;
		}

		// remote port
		int rport;
		// local port
		int lport;

		// remote address	
		sockaddr_in addr;
		hostent *he;

		// local address
		sockaddr_in laddr;
		hostent *lhe = NULL;

		instance->configReader->getValue(rport, "remoteserver", "port");
		instance->configReader->getValue(lport, "localbind", "port", 0);

		if (!instance->configReader->isEmpty("localbind", "address"))
		{
			lhe = gethostbyname((instance->configReader->getValue<String>("localbind", "address", String())).c_str());
			if (!lhe)
			{
				instance->error = String() + "Resolving local address: " + std::strerror(errno);
				return err::socketengine::resolve;
			}
		}

		he = gethostbyname((instance->configReader->getValue<String>("remoteserver", "address", String())).c_str());
		if (!he)
		{
			instance->error = String() + "Resolving remote address: " + std::strerror(errno);
			return err::socketengine::resolve;
		}

		fd = socket(PF_INET, SOCK_STREAM, 0);
		if (fd == -1)
		{
			instance->error = String() + "Creating socket: " + std::strerror(errno);
			return err::socketengine::socket;
		}

		addr.sin_family = AF_INET;
		addr.sin_port = htons(rport);
		addr.sin_addr = *(reinterpret_cast<in_addr*>(he->h_addr));
		std::memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

		if (lport)
		{
			laddr.sin_family = AF_INET;
			laddr.sin_port = htons(lport);

			if (instance->configReader->isEmpty("localbind", "address"))
				laddr.sin_addr.s_addr = INADDR_ANY;
			else
				laddr.sin_addr = *(reinterpret_cast<in_addr*>(lhe->h_addr));

			std::memset(laddr.sin_zero, '\0', sizeof(laddr.sin_zero));

			if (bind(fd, reinterpret_cast<sockaddr*>(&laddr), sizeof(laddr)) == -1)
			{
				instance->error = String() + "Binding to local address: " + std::strerror(errno);
				return err::socketengine::local;
			}
		}

		if (::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1)
		{
			instance->error = std::strerror(errno);
			return err::socketengine::remote;
		}

		FD_ZERO(&set);
		FD_SET(fd, &set);
		FD_ZERO(&eset);
		FD_SET(fd, &eset);

		connected = true;
		instance->debug("Connected.");

		return err::socketengine::none;
	}
	

	// send data to remote server
	ErrorCode send(String &buf)
	{
		if (buf.length() > 510)
			buf.resize(510);

		instance->debug("(SENT)  " + buf);
		buf += "\r\n";
		
		return send(buf.c_str(), buf.length());
	}


	// receive data from remote server
	ErrorCode recv(String &ret)
	{
		ret.clear();

		// Make copies of original sets
		fd_set st = set;
		fd_set ex = eset;

		int rv = select(fd + 1, &st, NULL, &ex, NULL);
		if (rv == -1)
		{
			instance->finalize(String() + "select() error: " + std::strerror(errno));
			return err::socketengine::engine;
		}
		else if (!rv)
			return err::socketengine::nobuf;

		else if (FD_ISSET(fd, &ex))
		{
			instance->finalize("Socket threw an exception.");
			return err::socketengine::socket;
		}
		else
		{
			char buf[2];
			buf[1] = 0;
			char ch = 0;

			while (true)
			{
				int r = ::recv(fd, buf, 1, 0);
				if (r == -1)
				{
					instance->finalize(String() + "recv() error: " + std::strerror(errno));
					return err::socketengine::recv;
				}
				else if (!r)
				{
					instance->finalize("Remote server closed connection.");
					return err::socketengine::closed;
				}

				if (ch == '\r' || ch == '\n')
				{
					ret.erase(ret.end() - 1);
					instance->debug("(RECV)  " + ret);
					return err::socketengine::none;
				}

				ch = buf[0];
				ret.append(buf);
			}
		}
		
		// should never reach here o.o
		return err::socketengine::none;
	}


	// disconnect
	ErrorCode disconnect()
	{
		if (!isConnected())
			return err::socketengine::notConnected;
		
		instance->debug("Disconnecting ...");
		
		if (shutdown(fd, SHUT_RDWR) == -1)
		{
			instance->error = String() + "shutdown() error: " + std::strerror(errno);
			return err::socketengine::shutdown;
		}

		if (close(fd) == -1)
		{
			instance->error = String() + "close() error: " + std::strerror(errno);
			return err::socketengine::close;
		}

		connected = false;
		instance->debug("Disconnected.");
		
		return err::socketengine::none;
	}
};


// initialize the select socketengine
initSocketEngine(SelectEngine)

