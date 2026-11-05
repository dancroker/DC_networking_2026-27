
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


    Fox_text = std::make_unique<sf::Text>(*font);
    Fox_text->setString("Play Fox");
    Fox_text->setPosition({ 1000,400 });

    Hound_text = std::make_unique<sf::Text>(*font);
    Hound_text->setString("Play Hound");
    Hound_text->setPosition({ 1000,700 });

    Player_text = std::make_unique<sf::Text>(*font);
    Player_text->setString("VOID!");
    Player_text->setPosition({ 1000,400 });
        
    return true;
}

void Game::update(float dt)
{
    std::vector<Move> moves = board->update();
    if(!moves.empty())
    {
        if(server != nullptr)
        {
            sf::Packet packet = server->packData("", moves.back().origin, moves.back().destination);
            server->input(packet);
        }
        else if (client != nullptr)
        {
            sf::Packet packet = client->packData("", moves.back().origin, moves.back().destination);
            client->input(packet);
        }
    }
}

void Game::render()
{
    board->render();  
    if (game_state == MENU)
    {
        window.draw(*host_text);
        window.draw(*join_text);

        if(player_selected == 1)
        {
            Player_text->setString("Fox");
        }
        else
        {
            Player_text->setString("Hound");
        }
    }
    //if (game_state == SELECTPLAYER)
    //{
    //    if (is_selecting)
    //    {
    //        Fox_text->setString("Play Fox");
    //        Hound_text->setString("Play Hound");
    //        window.draw(*Hound_text);
    //        window.draw(*Fox_text);
    //    }
    //    else 
    //    {
    //        Fox_text->setString("Wait For Character Select");
    //        Hound_text->setString("Wait For Character Select");
    //        window.draw(*Hound_text);
    //        window.draw(*Fox_text);
    //    }
    //}

}

void Game::mouseButtonPressed(const sf::Event::MouseButtonPressed* event)
{
    board->mouseButtonPressed(event, player_selected);

    if(game_state == MENU)
    {
        if(host_text->getGlobalBounds().contains(sf::Vector2f(event->position)))
        {
            game_state = GAME;
            std::cout << "CREATE SERVER" << std::endl;
            server = std::make_unique<Server>(board);
            server->init();
            player_selected = 0;
            server->run();
            is_selecting = true;
            
            return;
        }
        if (join_text->getGlobalBounds().contains(sf::Vector2f(event->position)))
        {
            game_state = GAME;
            std::cout << "CONNECTING..." << std::endl;
            client = std::make_unique<Client>(board);
            client->connect();
            player_selected = 1;
            client->run();
            return;
        }
    }
    //if (game_state == SELECTPLAYER && is_selecting)
    //{
    //    if (Fox_text->getGlobalBounds().contains(sf::Vector2f(event->position)))
    //    {
    //        game_state = GAME;
    //        player_selected = 1;
    //        server->input("Fox");
    //    }
    //    if (Hound_text->getGlobalBounds().contains(sf::Vector2f(event->position)))
    //    {
    //        game_state = GAME;
    //        player_selected = 2;
    //        server->input("Hound");
    //    }
    //}
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





