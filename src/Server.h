#pragma once

#include <SFML/Network.hpp>
#include <thread>
#include <vector>
#include <iostream>
#include <mutex>
#include <memory>

class Server
{
public:
	Server();
	~Server();
	void init();
	void run();
	void listen(sf::TcpSocket& c_socket);
	void input();
	void relay(std::string buffer, sf::TcpSocket& c_socket);

private:
	std::vector<std::thread> workers;
	std::vector<std::unique_ptr<sf::TcpSocket>> connections;

	std::unique_ptr<sf::TcpListener> listener;

	bool running = true;
	std::mutex mutex;
};
