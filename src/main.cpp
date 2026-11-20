#include <iostream>
#include <SFML/Graphics.hpp>
#include "Game.h"


int main()
{
  std::cout << "You should see a window that opens as well as this writing to console..."
            << std::endl;

  // create window and set up
  sf::RenderWindow window(sf::VideoMode({ 1350, 900 }), "NETWORK_fox_n_hounds");
  window.setFramerateLimit(60);

  //initialise an instance of the game class
  Game game(window);

  //run the init function of the game class and check it all initialises ok
  if (!game.init())
  {
    return 0;
  }

  // A Clock starts counting as soon as it's created
  sf::Clock clock;

  // Game loop: run the program as long as the window is open
  while (window.isOpen())
  {
    // check all the window's events that were triggered since the last iteration of the loop

    //calculate delta time
    sf::Time time = clock.restart();
    float dt = time.asSeconds();

    //'process inputs' element of the game loop
    while (const std::optional event = window.pollEvent())
    {
        //Close was requested, close the window
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        //A Key was pressed, handle it
        else if (const sf::Event::KeyPressed* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            game.keyPressed(keyPressed);
        }
        //A key was released, handle it
        else if (const sf::Event::KeyReleased* keyReleased = event->getIf<sf::Event::KeyReleased>())
        {
            game.keyReleased(keyReleased);
        }
        //The mouse moved, handle it
        else if (const sf::Event::MouseMoved* mouseMoved = event->getIf<sf::Event::MouseMoved>())
        {
            game.mouseMoved(mouseMoved);
        }
        //A mouse button was pressed, handle it
        else if (const sf::Event::MouseButtonPressed* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
        {
            game.mouseButtonPressed(mouseButtonPressed);
        }
        //A mouse button was released, handle it
        else if (const sf::Event::MouseButtonReleased* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>())
        {
            game.mouseButtonReleased(mouseButtonReleased);
        }
        if (event->is<sf::Event::TextEntered>())
        {
            auto text = event->getIf<sf::Event::TextEntered>();
            if(text->unicode < 128)
            {
                game.addToPlayerInput(static_cast<char>(text->unicode));
            }

        }
    }


    //'update' element of the game loop
    game.update(dt);

    window.clear(sf::Color::Black);

    //'render' element of the game loop
    game.render();
    window.display();
  }

  return 0;
}
