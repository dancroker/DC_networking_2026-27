#include "Client.h"

Client::Client()
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

		if (connected) 
		{
			socket->send(reinterpret_cast<char*>(str.data()), str.length());
		}
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
			char static_buffer[1028];
			while (connected)
			{
				std::size_t recived;
				sf::Socket::Status status = socket->receive(static_buffer, 1028, recived);

				if (status == sf::Socket::Status::Disconnected)
				{
					connected = false;
					std::cout << "CLEAN DISSCONNECT" << std::endl;
					socket->disconnect();
					break;
				}
				else
				{
					if (recived < 1028) static_buffer[recived] = '\0';
					std::cout << static_buffer << std::endl;
				}
			}
		}
		});
	connection_thread.detach();
}
