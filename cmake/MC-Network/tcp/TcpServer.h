#pragma once

#include <SDl/SDL_net.h>
#include <cstring>
#include <iostream>

namespace mc {
	namespace net
	{
		class TcpServer {
		private:
			TCPsocket m_server;
			TCPsocket m_client;
			IPaddress m_ip;
		public:
			TcpServer(int port);
			void accept();
			void destroy();
		};
	}
}