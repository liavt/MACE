#pragma once

#include <SDL/SDL_net.h>
#include <cstring>
#include <iostream>

namespace mc {
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