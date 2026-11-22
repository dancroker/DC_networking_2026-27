#pragma once

#include <SFML/Network.hpp>
#include <iostream>
#include <atomic>
#include <thread>
#include "Board.h"
#include <mutex>

class Client
{
public:
	Client(std::shared_ptr<Board> _board);
	~Client();
	void connect();
	void input(std::string str);
	void input(sf::Packet _packet);
	void run();
	sf::Packet packData(std::string _string = "", sf::Vector2i _origin = { -1,-1 }, sf::Vector2i _destination = { -1,-1 }) const;
	void unpackData(sf::Packet _packet, std::string& _string, sf::Vector2i& _origin, sf::Vector2i& _destination);
	std::atomic<bool> running = false;
	std::atomic<bool> connected = false;
	std::string getTextRecivedFromServer();

private:
	std::unique_ptr<sf::TcpSocket> socket;
	std::shared_ptr<Board> board;
	textRecived text_recieved;
};
