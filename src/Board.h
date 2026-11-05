#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Tile.h"
#include "Player.h"

enum Colour // This ordering is important!
{
	RED,
	BLACK
};

struct Move 
{
public:
	sf::Vector2i origin;
	sf::Vector2i destination;
};

class Board
{
public:
	Board(sf::RenderWindow& _window);
	void init();
	void render();
	std::vector<Move> update();
	void mouseButtonPressed(const sf::Event::MouseButtonPressed* event, int player);
	void mouseButtonReleased(const sf::Event::MouseButtonReleased* event);
	void mouseMoved(const sf::Event::MouseMoved* event);
	void endTurn();
	void moveByCoords(sf::Vector2i _origin, sf::Vector2i _destination);

private:
	sf::RenderWindow& window;

	//Board implementation
	sf::Vector2i grid_size;
	sf::Vector2f grid_origin;
	std::vector<std::shared_ptr<Tile>> grid;
	std::unique_ptr<sf::Texture> board_texture;
	std::unique_ptr<sf::Sprite> board_sprite;

	std::unique_ptr<sf::Font> font;
	std::vector<std::unique_ptr<sf::Text>> board_column_nums;
	std::vector<std::unique_ptr<sf::Text>> board_row_nums;

	//Pieces
	std::vector<std::unique_ptr<sf::Texture>> piece_textures;
	sf::Vector2f piece_size_target;
	sf::Vector2f piece_origin;

	//Players to own the pieces
	std::vector<std::unique_ptr<Player>> players;

	//Mouse interactions with the board & pieces Maybe change these to weak pointers
	std::shared_ptr<Piece> held_piece = nullptr;
	std::shared_ptr<Tile> hover_tile = nullptr;

	int current_turn;

	std::vector<Move> moves;
};