
#include "Game.h"
#include "Tile.h"
#include <iostream>

Game::Game(sf::RenderWindow& game_window)
  : window(game_window)
{
  
}

Game::~Game()
{
    
}

bool Game::init()
{
    board = std::make_shared<Board>(window);
    board->init();  

    font = std::make_unique<sf::Font>("../Data/Fonts/OpenSans-Bold.ttf");
    host_text = std::make_unique<sf::Text>(*font);
    host_text->setString("Host Game");
    host_text->setPosition({ 1000,400 });
   
    join_text = std::make_unique<sf::Text>(*font);
    join_text->setString("Join Game");
    join_text->setPosition({ 1000,700 });
        
    return true;
}

void Game::update(float dt)
{

}

void Game::render()
{
    board->render();  
    if (game_state == MENU)
    {
        window.draw(*host_text);
        window.draw(*join_text);
    }
}

void Game::mouseButtonPressed(const sf::Event::MouseButtonPressed* event)
{
    board->mouseButtonPressed(event);

    if(game_state == MENU)
    {
        if(host_text->getGlobalBounds().contains(sf::Vector2f(event->position)))
        {
            game_state = GAME;
            std::cout << "CREATE SERVER" << std::endl;
            server = std::make_unique<Server>();
            server->init();
            server->run();
        }
        if (join_text->getGlobalBounds().contains(sf::Vector2f(event->position)))
        {
            game_state = GAME;
            std::cout << "CONNECTING..." << std::endl;
            client = std::make_unique<Client>();
            client->connect();
            client->run();
        }
    }
}

void Game::mouseButtonReleased(const sf::Event::MouseButtonReleased* event)
{
    board->mouseButtonReleased(event);    
}

void Game::keyPressed(const sf::Event::KeyPressed* event)
{

}

void Game::keyReleased(const sf::Event::KeyReleased* event)
{

}

void Game::mouseMoved(const sf::Event::MouseMoved* event)
{
    board->mouseMoved(event);
}




