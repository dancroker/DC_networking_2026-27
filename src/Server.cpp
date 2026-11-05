#include "Server.h"

Server::Server()
{
}

Server::~Server()
{
}

void Server::init()
{
	if(listener == nullptr)
	{
		listener = std::make_unique<sf::TcpListener>();
	}

	if(listener->listen(53000) != sf::Socket::Status::Done)
	{
		std::cout << "NO BIND LISTENER TO PORT 53000" << std::endl;
	}
}

void Server::run()
{
	std::thread input_thread([&] {input();});
	input_thread.detach();

	std::thread connections_thread([&] {
		while (running)
		{
			sf::TcpSocket& c_sock = *connections.emplace_back(std::make_unique<sf::TcpSocket>());
			if (listener->accept(c_sock) != sf::Socket::Status::Done)
			{
				connections.pop_back();
				std::cout << "Connection failed on server side" << std::endl;
				continue;
			}

			std::cout << "Client connected @ " << c_sock.getRemotePort() << std::endl;

			workers.emplace_back([&]
				{
					listen(c_sock);
					std::cout << "Dissconnect?!?" << std::endl;
					std::lock_guard<std::mutex> lck(mutex);

					for (int i = 0; i < connections.size(); i++)
					{
						if (connections[i]->getRemotePort() == c_sock.getRemotePort())
						{
							connections.erase(std::next(connections.begin(), i));
							break;
						}
					}
				});
		}
		});
	connections_thread.detach();
}

void Server::listen(sf::TcpSocket& c_socket)
{
	bool continue_receving = true;

	char data[1028];
	std::size_t recieved;

	while(continue_receving)
	{
		sf::Socket::Status status = c_socket.receive(data, 1028, recieved);

		if(status == sf::Socket::Status::Disconnected)
		{
			continue_receving = false;
			std::cout << "dissconnected" << std::endl;
		}

		if (recieved < 1028) { data[recieved] = '\0'; }
		relay(data, c_socket);

		std::cout << reinterpret_cast<char*>(data) << std::endl;
	}
	c_socket.disconnect();
}

void Server::input()
{
	while (running)
	{
		std::string str;
		std::getline(std::cin, str);
		std::cin.clear();

		std::lock_guard<std::mutex> lck(mutex);
		for(std::unique_ptr<sf::TcpSocket>& connection : connections)
		{
			connection->send(reinterpret_cast<char*>(str.data()), str.length());
		}
	}
}

void Server::relay(std::string buffer, sf::TcpSocket& c_socket)
{
	char* message = reinterpret_cast<char*>(buffer.data());

	std::lock_guard<std::mutex> lck(mutex);

	for (std::unique_ptr<sf::TcpSocket>& connection : connections)
	{
		if(connection->getRemotePort() != c_socket.getRemotePort())
		{
			connection->send(message, buffer.size());
		}
	}
}
