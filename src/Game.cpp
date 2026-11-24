
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
    host_text->setPosition({ 1000,600 });
   
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

    Chat_text = std::make_unique<sf::Text>(*font);
    Chat_text->setString("VOID!");
    Chat_text->setPosition({ 820.0,20.0 });
        
    PlayerInput_text = std::make_unique<sf::Text>(*font);
    PlayerInput_text->setString("VOID!");
    PlayerInput_text->setPosition({ 820,380 });



    ChatBox_Rect = std::make_unique<sf::RectangleShape>(sf::Vector2f{ 515,400 });
    ChatBox_Rect->setFillColor(sf::Color{ 50,50,50 });
    ChatBox_Rect->setPosition({ 820.0, 20.0 });
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
    if (getPlayerInput().length() > 0)
    {
        sf::String text = getPlayerInput();
        PlayerInput_text->setString(text);
    }
    else
    {
        PlayerInput_text->setString("Enter Text...");
    }

    if (client != nullptr) 
    {
        std::string responce = client->getTextRecivedFromServer();
        if (responce != "")
        {
            std::string new_line = "Svr: " + responce;
            chat_text_lines.push_back(new_line);
            if (chat_text_lines.size() > 9)
            {
                text_input_index++;
            }

        }
    }
    if (server != nullptr)
    {
        std::string responce = server->getTextRecivedFromServer();
        if (responce != "")
        {
            std::string new_line = "Cli: " + responce;
            chat_text_lines.push_back(new_line);
            if (chat_text_lines.size() > 9)
            {
                text_input_index++;
            }
        }
    }

    Chat_text->setString("");
    for(int i = text_input_index; i< text_input_index + 9; i++)
    {
        if (i < chat_text_lines.size())
        {
            std::string new_line;
            new_line = Chat_text->getString() + chat_text_lines[i] + "\n";
            Chat_text->setString(new_line);
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
    //std::cout << getPlayerInput() << std::endl;
    window.draw(*ChatBox_Rect);
    window.draw(*PlayerInput_text);
    window.draw(*Chat_text);
    

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
    if (event->scancode == sf::Keyboard::Scancode::Delete) // backspace dont work?!
    {
        if (getPlayerInput().length() > 0) 
        {

            setPlayerInput(getPlayerInput().substr(0, getPlayerInput().size() - 1));
        }
    }
    if (event->scancode == sf::Keyboard::Scancode::Enter) // backspace dont work?!
    {
        if(server != nullptr)
        {
            std::string new_line = "Svr: " + playerInput;
            chat_text_lines.push_back(new_line);
            server->input(playerInput);
            setPlayerInput("");
            if (chat_text_lines.size() > 9)
            {
                text_input_index++;
            }
        }
        else if (client != nullptr)
        {
            std::string new_line = "Cli: " + playerInput;
            chat_text_lines.push_back(new_line);
            client->input(playerInput);
            if (chat_text_lines.size() > 9)
            {
                text_input_index++;
            }
            setPlayerInput("");
        }
        else
        {
            setPlayerInput("");
        }
    }
    if (event->scancode == sf::Keyboard::Scancode::Up)
    {
        if (text_input_index > 0)
        {
            text_input_index--;
        }
    }
    if (event->scancode == sf::Keyboard::Scancode::Down)
    {
        if(text_input_index+9 < chat_text_lines.size())
        {
            text_input_index++;
        }
    }
}

void Game::keyReleased(const sf::Event::KeyReleased* event)
{

}

void Game::mouseMoved(const sf::Event::MouseMoved* event)
{
    board->mouseMoved(event);
}





