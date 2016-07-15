#include "TcpServer.h"

namespace mc {
	namespace net
	{
		TcpServer::TcpServer(int port) {
			SDLNet_ResolveHost(&m_ip, NULL, port);

			m_server = SDLNet_TCP_Open(&m_ip);
		}

		void TcpServer::accept() {
			while (1) {
				m_client = SDLNet_TCP_Accept(m_server);

				const char* text = "hello!\n";
				if (m_client) {
					SDLNet_TCP_Send(m_client, text, strlen(text));
					SDLNet_TCP_Close(m_client);
					break;
				}
			}
		}

		void TcpServer::destroy() {
			SDLNet_TCP_Close(m_server);
		}
	}
}