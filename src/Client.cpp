#include "Client.h"

Client::Client(std::shared_ptr<Board> _board) : board(_board)
{
}

Client::~Client()
{
}

void Client::connect()
{
	if (socket == nullptr)
	{
		socket = std::make_unique<sf::TcpSocket>();
	}
	if (socket->connect(*sf::IpAddress::fromString("127.0.0.1"),53000) == sf::Socket::Status::Done)
	{
		std::cout << "CONNECTED!" << std::endl;
		connected = true;
	}
	else
	{
		std::cout << "FAILED CONNECTION!" << std::endl;
	}
}

void Client::input()
{
	while(running)
	{
		std::string str;
		std::getline(std::cin, str);
		std::cin.clear();

		sf::Packet packet;
		packet = packData(str.data());

		if (connected) 
		{
			socket->send(packet);
		}
	}
}

void Client::input(sf::Packet _packet)
{
		sf::Packet packet;
		packet = _packet;

		if (connected)
		{
			socket->send(packet);
		}
}

void Client::run()
{
	running = true;
	std::thread input_thread([&] {input();});
	input_thread.detach();

	std::thread connection_thread([&] {
		while (running)
		{
			//char static_buffer[1028];
			sf::Packet packet;

			while (connected)
			{
				std::size_t recived;
				//sf::Socket::Status status = socket->receive(static_buffer, 1028, recived);
				sf::Socket::Status status = socket->receive(packet);

				if (status == sf::Socket::Status::Disconnected)
				{
					connected = false;
					std::cout << "CLEAN DISSCONNECT" << std::endl;
					socket->disconnect();
					break;
				}
				else
				{
					//if (recived < 1028) static_buffer[recived] = '\0';
					//std::cout << static_buffer << std::endl;

					std::string string;
					sf::Vector2i origin;
					sf::Vector2i destination;

					unpackData(packet, string, origin, destination);

					if(string.length() > 0)
					{
						std::cout << string << std::endl;
					}
					if(origin.x >= 0 && destination.x >= 0)
					{
						board->moveByCoords(origin, destination);
					}
					
				}
			}
		}
		});
	connection_thread.detach();
}

sf::Packet Client::packData(std::string _string, sf::Vector2i _origin, sf::Vector2i _destination) const
{
	sf::Packet packet;
	packet << _string << _origin.x << _origin.y << _destination.x << _destination.y;
	return packet;
}

void Client::unpackData(sf::Packet _packet, std::string& _string, sf::Vector2i& _origin, sf::Vector2i& _destination)
{
	_packet >> _string >> _origin.x >> _origin.y >> _destination.x >> _destination.y;
}
