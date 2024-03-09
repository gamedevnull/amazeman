#include <iostream>

#include <SFML/Graphics.hpp>
using namespace sf;

#include "include/globals.hpp"

class GameInput
{
public:
    int moveX, moveY;
    bool keyW, keyS, keyA, keyD;
    GameInput() : moveX(0), moveY(0), keyW(0), keyS(0), keyA(0), keyD(0) {}
};

class AmazeManGame
{

public:
    RenderWindow window;

    const char *mazeMapAsString;
    int **mazeMap;
    int mapCols, mapRows;
    int tileWidth, tileHeight;

    int gameViewWidth, gameViewHeight, gameViewOffsetX, gameViewOffsetY, gameViewRow, gameViewCol, gameViewRows, gameViewCols;

    int boardViewRow, boardViewCol;

    int playerX, playerY;

    GameInput input;

    AmazeManGame() : window(VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "A maze man!")
    {
        window.setFramerateLimit(60);

        mazeMapAsString = "1111111111111111111111111111111110000000000000000000000000100001101111111111111111111111111011011011000000000000000010001000110110110111111111111111101010111101101101111111111111100010101111011011011110000000000011101011000110100000001111111111111010110101101011111110000000000010001101011010111111101111101110111111010110001111111011111011101111110101101111111110111110111011111101011011111111101111101110111111010110001000000011111011100000000101111010111110111110111111111101011110101111100000000000000000010110101011111011111011111111010101101010111110111110100000010101011010101111111111101011110101010110101000000000011010111100010101100010111111110110101111110101111111101111111101100011111101010110000010000111111011111111010101101111101100000000111111110101011011111011111111111111100001010110111110001000000000000001110101100000001010101111111110000000011111111110101011111111101111111111100000001010000000000011111111111011111110111111111110111111111110000000000000001111100000000111111111111111111111111111111111";

        mapCols = 32;
        mapRows = 32;

        tileWidth = 32;
        tileHeight = 32;

        gameViewWidth = 10 * tileWidth;
        gameViewHeight = 10 * tileHeight;

        gameViewRows = gameViewHeight / tileHeight;
        gameViewCols = gameViewWidth / tileWidth;

        gameViewRow = 2;
        gameViewCol = 2;

        gameViewOffsetX = gameViewRow * tileWidth;
        gameViewOffsetY = gameViewCol * tileHeight;

        boardViewRow = 0;
        boardViewCol = 0;

        playerX = 1 * tileWidth;
        playerY = 1 * tileHeight;

        mazeMap = new int *[mapRows];
        for (int i = 0; i < mapRows; ++i)
        {
            mazeMap[i] = new int[mapCols];
        }

        int idx = 0;
        for (int i = 0; i < mapRows; ++i)
        {
            for (int j = 0; j < mapCols; ++j)
            {
                mazeMap[i][j] = mazeMapAsString[idx++] - '0'; // char to int
            }
        }
    }

    void Start()
    {

        while (window.isOpen())
        {

            Event e;
            while (window.pollEvent(e))
            {

                switch (e.type)
                {
                case Event::Closed:
                    window.close();
                    break;

                case sf::Event::KeyPressed:
                    switch (e.key.code)
                    {
                    case sf::Keyboard::Left:
                        input.moveX = -1;
                        break;
                    case sf::Keyboard::Right:
                        input.moveX = 1;
                        break;
                    case sf::Keyboard::Up:
                        input.moveY = -1;
                        break;
                    case sf::Keyboard::Down:
                        input.moveY = 1;
                        break;
                    case sf::Keyboard::W:
                        input.keyW = 1;
                        break;
                    case sf::Keyboard::S:
                        input.keyS = 1;
                        break;
                    case sf::Keyboard::A:
                        input.keyA = 1;
                        break;
                    case sf::Keyboard::D:
                        input.keyD = 1;
                        break;
                    default:
                        break;
                    }
                    break;

                case sf::Event::KeyReleased:
                    switch (e.key.code)
                    {
                    case sf::Keyboard::Left:
                        input.moveX = 0;
                        break;
                    case sf::Keyboard::Right:
                        input.moveX = 0;
                        break;
                    case sf::Keyboard::Up:
                        input.moveY = 0;
                        break;
                    case sf::Keyboard::Down:
                        input.moveY = 0;
                        break;
                    case sf::Keyboard::W:
                        input.keyW = 0;
                        break;
                    case sf::Keyboard::S:
                        input.keyS = 0;
                        break;
                    case sf::Keyboard::A:
                        input.keyA = 0;
                        break;
                    case sf::Keyboard::D:
                        input.keyD = 0;
                        break;
                    default:
                        break;
                    }
                    break;

                default:
                    break;
                }
            }

            Update();
            Render();
        }
    }

    void Update()
    {
        // MOVE PLAYER

        if (input.moveX > 0)
        {
            playerX++;
        }
        else if (input.moveX < 0)
        {
            playerX--;
        }

        if (input.moveY > 0)
        {
            playerY++;
        }
        else if (input.moveY < 0)
        {
            playerY--;
        }

        // MOVE MAP (debug)

        if (input.keyD)
        {
            if (boardViewCol < mapCols - gameViewCols)
            {
                boardViewCol++;
            }
            input.keyD = 0;
        }

        if (input.keyA)
        {
            if (boardViewCol > 0)
            {
                boardViewCol--;
            }
            input.keyA = 0;
        }

        if (input.keyS)
        {
            if (boardViewRow < mapRows - gameViewRows)
            {
                boardViewRow++;
            }
            input.keyS = 0;
        }

        if (input.keyW)
        {
            if (boardViewRow > 0)
            {
                boardViewRow--;
            }
            input.keyW = 0;
        }
    }

    void Render()
    {
        window.clear(Color(0, 0, 0));

        renderMazeMap();

        renderGameViewFrame();

        // renderCompleteMazeMapWithoutAnyOffsets();

        renderPlayer();

        window.display();
    }

    void renderPlayer()
    {
        sf::RectangleShape tile;
        tile.setSize(sf::Vector2f(tileWidth, tileHeight));
        tile.setFillColor(sf::Color(255, 0, 0));
        tile.setPosition(gameViewOffsetX + playerX, gameViewOffsetY + playerY);
        window.draw(tile);
    }

    void renderTile(int x, int y, int w, int h)
    {
        sf::RectangleShape tile;
        tile.setSize(sf::Vector2f(w, h));
        tile.setFillColor(sf::Color(0, 0, 255));
        tile.setPosition(x, y);
        window.draw(tile);
    }

    void renderGameViewFrame()
    {
        sf::RectangleShape rectangle(sf::Vector2f(gameViewWidth, gameViewHeight));
        rectangle.setPosition(gameViewOffsetX, gameViewOffsetY);
        rectangle.setFillColor(sf::Color::Transparent);
        rectangle.setOutlineThickness(1);
        rectangle.setOutlineColor(sf::Color::Red);
        window.draw(rectangle);
    }

    void renderMazeMap()
    {
        std::cout << "board offset is: " << boardViewRow << " " << boardViewCol << std::endl;

        bool debug = 1;

        if (debug)
        {
            // shows +1 extra row/col outside gameView box
            for (int i = 0; i < gameViewRows + 2; ++i)
            {
                for (int j = 0; j < gameViewCols + 2; ++j)
                {
                    if ((boardViewRow - 1 + i >= 0) and (boardViewCol - 1 + j >= 0) and (boardViewCol - 1 + j < mapCols) // if mapCols is 32 then max j is 31 !!!
                        and (boardViewRow - 1 + i < mapRows))
                    {

                        if (mazeMap[boardViewRow - 1 + i][boardViewCol - 1 + j] == 1)
                        {
                            renderTile(
                                gameViewOffsetX - (tileWidth) + (j * tileWidth),
                                gameViewOffsetY - (tileWidth) + (i * tileHeight),
                                tileWidth, tileHeight);
                        }
                    }
                }
            }
        }
        else
        {
            for (int i = 0; i < gameViewRows; ++i)
            {
                for (int j = 0; j < gameViewCols; ++j)
                {
                    if (mazeMap[boardViewRow + i][boardViewCol + j] == 1)
                    {
                        renderTile(
                            gameViewOffsetX + (j * tileWidth),
                            gameViewOffsetY + (i * tileHeight),
                            tileWidth, tileHeight);
                    }
                }
            }
        }
    }

    void renderCompleteMazeMapWithoutAnyOffsets()
    {
        for (int i = 0; i < mapRows; ++i)
        {
            for (int j = 0; j < mapCols; ++j)
            {

                if (mazeMap[i][j] == 1)
                {
                    renderTile(j * tileWidth, i * tileHeight, tileWidth, tileHeight);
                }
            }
        }
    }

    ~AmazeManGame()
    {
        int n = 4;
        for (int i = 0; i < n; ++i)
        {
            delete[] mazeMap[i];
        }
        delete[] mazeMap;
    }
};

int main()
{
    AmazeManGame game;
    game.Start();
    return 0;
}