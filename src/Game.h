
#pragma once

#include <SFML/Graphics.hpp>
#include "Board.h"
#include "Server.h"
#include "Client.h"

class Game
{
 public:
	Game(sf::RenderWindow& window);
	~Game();
	bool init();
	void update(float dt);
	void render();
	void mouseButtonPressed(const sf::Event::MouseButtonPressed* event);
	void mouseButtonReleased(const sf::Event::MouseButtonReleased* event);
	void keyPressed(const sf::Event::KeyPressed* event);
	void keyReleased(const sf::Event::KeyReleased* event);
	void mouseMoved(const sf::Event::MouseMoved* event);

 private:
	sf::RenderWindow& window;

	std::shared_ptr<Board> board;

	std::unique_ptr<Server> server;
	std::unique_ptr<Client> client;

	std::unique_ptr<sf::Font> font;
	std::unique_ptr<sf::Text> host_text;
	std::unique_ptr<sf::Text> join_text;

	enum
	{
		MENU,
		GAME,
	};
	int game_state = MENU;

};

