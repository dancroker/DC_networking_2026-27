#pragma once

#include <SFML/Network.hpp>
#include <iostream>
#include <atomic>
#include <thread>

class Client
{
public:
	Client();
	~Client();
	void connect();
	void input();
	void run();
	std::atomic<bool> running = false;
	std::atomic<bool> connected = false;

private:
	std::unique_ptr<sf::TcpSocket> socket;
};
