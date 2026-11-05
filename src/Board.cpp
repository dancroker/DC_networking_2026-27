#include <iostream>
#include "Board.h"

Board::Board(sf::RenderWindow& _window) : window(_window)
{
    piece_size_target = { 0,0 };
    current_turn = 0;
}


void Board::init()
{
    sf::Vector2u window_size = window.getSize();
    float tile_size = floor(window_size.y / 10); // Board tiles are 10% of the window height
    piece_size_target = { floor(tile_size * 9 / 10), floor(tile_size * 9 / 10) }; // Piece sprites are 90% of the tile size

    //Create a grid of Tiles to represent our board
    grid_size = { 8, 8 };
    grid_origin = { tile_size, tile_size };
    grid.reserve(grid_size.x * grid_size.y);
    for (int i = 0; i < grid_size.x; i++)
    {
        for (int j = 0; j < grid_size.y; j++)
        {
            grid.emplace_back(std::make_shared<Tile>(Tile(tile_size, { j * tile_size + grid_origin.x,i * tile_size + grid_origin.y }, { j,i })));
        }
    }

    // Set up board sprite
    board_texture = std::make_unique<sf::Texture>(sf::Texture("../Data/chess_board.png"));
    board_sprite = std::make_unique<sf::Sprite>(*board_texture);
    board_sprite->setPosition({ grid_origin.x, grid_origin.y });

    // Make sure board sprite is the same size as the grid we use as our board
    sf::Vector2f grid_pixel_size = { grid_size.x * tile_size,grid_size.y * tile_size }; //Grid size in pixels
    sf::Vector2f board_scale_factor = { grid_pixel_size.x / board_sprite->getGlobalBounds().size.x,  // Calculate scale factor
                                        grid_pixel_size.y / board_sprite->getGlobalBounds().size.y };
    board_sprite->scale(board_scale_factor); // Scale sprite by calculated factor


    //Populate vectors of text to show board co-ordinate system
    font = std::make_unique<sf::Font>("../Data/Fonts/OpenSans-Bold.ttf");
    board_column_nums.reserve(grid_size.x);
    for (int i = 0; i < grid_size.x; i++)
    {
        board_column_nums.push_back(std::make_unique<sf::Text>(*font, std::to_string(i), tile_size / 3));
        board_column_nums.back()->setOrigin({ board_column_nums.back()->getGlobalBounds().size.x / 2,board_column_nums.back()->getGlobalBounds().size.y });
        board_column_nums.back()->setPosition(sf::Vector2f(grid_origin.x + (tile_size * i) + tile_size / 2, grid_origin.y - tile_size / 4));
    }
    board_row_nums.reserve(grid_size.y);
    for (int i = 0; i < grid_size.y; i++)
    {
        board_row_nums.push_back(std::make_unique<sf::Text>(*font, std::to_string(i), tile_size / 3));
        board_row_nums.back()->setOrigin({ board_row_nums.back()->getGlobalBounds().size.x,board_row_nums.back()->getGlobalBounds().size.y / 2 });
        board_row_nums.back()->setPosition(sf::Vector2f(grid_origin.x - tile_size / 5, grid_origin.y + (tile_size * i) + tile_size / 2));
    }

    // Load piece textures, red then black, as RED is 0 and BLACK is 1
    piece_textures.reserve(2);
    piece_textures.emplace_back(std::make_unique<sf::Texture>("../Data/red_piece.png"));
    piece_textures.emplace_back(std::make_unique<sf::Texture>("../Data/black_piece.png"));

    // Sprite positions are calculated based on the sprite's origin (defaults at (0,0)). 
    // Variable below is the pixel location of the centre of the piece in the sprite, where we want to calculate positions from
    piece_origin = { 15,14 };

    // Make players, red then black, as RED is 0 and BLACK is 1
    players.reserve(2);
    players.emplace_back(std::make_unique<Player>(RED));
    players.emplace_back(std::make_unique<Player>(BLACK));

    // Add the fox (red) piece and place it in position 3,7 on the board
    players[RED]->addPiece(std::make_shared<Piece>(*piece_textures[RED], RED, piece_size_target, piece_origin));
    grid[3 + 7 * grid_size.x]->dropPiece(players[RED]->getPieces().back());

    // Add the hound (black) pieces and place them on the light squares along the top row
    players[BLACK]->getPieces().reserve(4);
    for (int i = 0; i < 7; i += 2)
    {
        players[BLACK]->addPiece(std::make_shared<Piece>(*piece_textures[BLACK], BLACK, piece_size_target, piece_origin));
        grid[i]->dropPiece(players[BLACK]->getPieces().back());
    }

    //Start at turn 0, players will go in order from lowest to highest position in the players vector
    current_turn = 0;
}

void Board::mouseButtonPressed(const sf::Event::MouseButtonPressed* event, int player)
{
    if (hover_tile != nullptr && hover_tile->getPiece() != nullptr)
    {
        if (hover_tile->getPiece()->getPlayerID() == current_turn && player == current_turn)
        {
            hover_tile->clicked(event, held_piece);
        }
    }
}

void Board::mouseButtonReleased(const sf::Event::MouseButtonReleased* event)
{
    if (held_piece != nullptr)
    {
        std::shared_ptr<Piece> piece = held_piece;
        held_piece = nullptr; // Make sure the mouse gives up the piece before the tile takes over it, in case the mouse moves and changes the piece's position
        if (hover_tile != nullptr)
        {
            if (!hover_tile->dropPiece(piece)) // If the piece can't be dropped into the tile we're hovering over
            {
                // Quick check to see if the piece has a valid origin - not exhaustive, but good enough for now
                if (piece->getOrigin().x >= 0)
                {
                    //If valid origin, put it back where it came from
                    grid[piece->getOrigin().x + grid_size.x * piece->getOrigin().y]->dropPiece(piece);
                }
                else
                {
                    //If no valid origin, destroy it
                    players[piece->getPlayerID()]->removePiece(piece);
                }
            }
            else if (piece->getOrigin() != piece->getDestination())
            {
                std::cout << "Piece moved from (" << piece->getOrigin().x << "," << piece->getOrigin().y
                    << ") to (" << piece->getDestination().x << "," << piece->getDestination().y << ")." << std::endl;
                //If the piece has been placed, and it's not where it came from, end the turn

                Move new_move;
                new_move.origin = piece->getOrigin();
                new_move.destination = piece->getDestination();
                moves.emplace_back(new_move);
                endTurn();
            }
        }
        else
        {
            grid[piece->getOrigin().x + grid_size.x * piece->getOrigin().y]->dropPiece(piece);
        }
    }
}

void Board::mouseMoved(const sf::Event::MouseMoved* event)
{
    //Function for dragging around a held piece
    if (held_piece != nullptr)
    {
        held_piece->getSprite().setPosition(sf::Vector2f(event->position));
    }

    // Function for monitoring which board tile the mouse is hovering over
    if (board_sprite->getGlobalBounds().contains(sf::Vector2f(event->position)))
    {
        sf::Vector2i position; // Relative position of the mouse compared to the top left corner of the grid - acts as x & y index for which tile is being hovered over
        position.x = floor((event->position.x - grid_origin.x) / grid[0]->getSize());
        position.y = floor((event->position.y - grid_origin.y) / grid[0]->getSize());

        if (hover_tile != grid[position.y * grid_size.x + position.x]) // If we're hovering over a new tile
        {
            if (hover_tile != nullptr) // If we used to be hovering over a tile
            {
                hover_tile->setHover(false); // Tell the previous tile we're not hovering over it anymore
            }

            hover_tile = grid[position.y * grid_size.x + position.x]; //Set our hover pointer to the tile we are hovering over
            hover_tile->setHover(true); // Tell that tile we're hovering over it
        }

    }
    else
    {
        // If we're not hovering over the board, clear hovering pointer & tell any previously hovered tile it's not being hovered any more
        if (hover_tile != nullptr)
            hover_tile->setHover(false);

        hover_tile = nullptr;
    }
}

void Board::render()
{
    window.draw(*board_sprite);

    for (std::unique_ptr<sf::Text>& text : board_row_nums)
    {
        window.draw(*text);
    }
    for (std::unique_ptr<sf::Text>& text : board_column_nums)
    {
        window.draw(*text);
    }

    for (std::shared_ptr<Tile> tile : grid)
    {
        window.draw(tile->getShape());
    }

    for (std::unique_ptr<Player>& player : players)
    {
        for (std::shared_ptr<Piece> piece : player->getPieces())
        {
            if (piece != held_piece)
            {
                window.draw(piece->getSprite());
            }
        }
    }

    if (held_piece != nullptr)
    {
        window.draw(held_piece->getSprite());
    }
}

std::vector<Move> Board::update()
{
    std::vector<Move> to_send;
    if(!moves.empty())
    {
        to_send.emplace_back(moves.back());
        moves.clear();
    }
    return to_send;
}

void Board::endTurn()
{
    current_turn++;
    if (current_turn >= players.size())
    {
        current_turn = 0;
    }
}

void Board::moveByCoords(sf::Vector2i _origin, sf::Vector2i _destination)
{
    Tile& _tile = *grid[_destination.x + (_destination.y * grid_size.x)];
    grid[_origin.x + (_origin.y * grid_size.x)].get()->movePiece(_tile);
    endTurn();
    std::cout << "Hi";
}