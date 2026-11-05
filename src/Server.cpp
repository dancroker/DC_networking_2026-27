#include "Server.h"

Server::Server(std::shared_ptr<Board> _board) : board(_board)
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

	//char data[1028];
	sf::Packet packet;

	std::size_t recieved;

	while(continue_receving)
	{
		//sf::Socket::Status status = c_socket.receive(data, 1028, recieved);
		sf::Socket::Status status = c_socket.receive(packet);

		if(status == sf::Socket::Status::Disconnected)
		{
			continue_receving = false;
			std::cout << "dissconnected" << std::endl;
		}


		//if (recieved < 1028) { data[recieved] = '\0'; }
		relay(packet, c_socket);

		std::string string;
		sf::Vector2i origin;
		sf::Vector2i destination;

		unpackData(packet, string, origin, destination);

		if (string.length() > 0)
		{
			std::cout << string << std::endl;
		}
		if (origin.x >= 0 && destination.x >= 0)
		{
			board->moveByCoords(origin, destination);
		}
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

		sf::Packet packet;
		packet = packData(str.data());

		std::lock_guard<std::mutex> lck(mutex);
		for(std::unique_ptr<sf::TcpSocket>& connection : connections)
		{
			connection->send(packet);
		}
	}
}

void Server::input(sf::Packet _packet)
{

		sf::Packet packet;
		packet = _packet;

		std::lock_guard<std::mutex> lck(mutex);
		for (std::unique_ptr<sf::TcpSocket>& connection : connections)
		{
			connection->send(packet);
		}
}

void Server::relay(sf::Packet buffer, sf::TcpSocket& c_socket)
{
	//char* message = reinterpret_cast<char*>(buffer.data());
	std::string string;
	sf::Vector2i origin;
	sf::Vector2i destination;

	unpackData(buffer, string, origin, destination);


	std::lock_guard<std::mutex> lck(mutex);

	for (std::unique_ptr<sf::TcpSocket>& connection : connections)
	{
		if(connection->getRemotePort() != c_socket.getRemotePort())
		{
			connection->send(buffer);
			//connection->send(buffer, buffer.size());
		}
	}
}

sf::Packet Server::packData(std::string _string, sf::Vector2i _origin, sf::Vector2i _destination) const
{
	sf::Packet packet;
	packet << _string << _origin.x << _origin.y << _destination.x << _destination.y;
	return packet;
}

void Server::unpackData(sf::Packet _packet, std::string& _string, sf::Vector2i& _origin, sf::Vector2i& _destination)
{
	_packet >> _string >> _origin.x >> _origin.y >> _destination.x >> _destination.y;
}
