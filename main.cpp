#include <iostream>
#include <SFML/Graphics.hpp>
using namespace sf;
#include "include/globals.hpp"
#include "include/Player.hpp"

class GameInput
{
public:
    int moveX, moveY;
    bool keyW, keyS, keyA, keyD, keyX;
    GameInput() : moveX(0), moveY(0), keyW(0), keyS(0), keyA(0), keyD(0), keyX(0) {}
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
    int playerScrollX, playerScrollY;

    int scrollHorizontalLow, scrollHorizontalHigh;
    int scrollVerticalLow, scrollVerticalHigh;

    GameInput input;

    bool debug_mode;

    Texture *bgTilesTexture;
    Vector2u bgTextureSize;

    Texture *playerTexture;
    Player *player;

    Sprite bgSprite;

    IntRect frameSelection;

    AmazeManGame() : window(VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "A maze man!")
    {
        window.setFramerateLimit(60);

        debug_mode = 0;

        // 16x16 + 4char-numbered sprites
        mazeMapAsString = "0001002100210021002100290029002100300031002100210021002100210021000100000000000000000000000000000000000000000000000000000000002100010000001700170017001700000017000000130000001300130013000000210001000000170000000000000000001700000013000000000000001300000021000100000017000000170017001700170000001300000000000000130000002100010000001700000017001700170017000000130013001300130013000000210021000000170000001700170017001700000000000000000000000000000021002100000017000000000000000000000000002500260025000000000000002100240000001700000000000000000000000000270000002800000000000000210021000000170000000000000000000000000025000000270000000000000022002100000017001700170017001700170017001700000017001700170000002100210000000000000000000000000000000000000000000000000000000000210022001700170017003700370037003700370037003700370037003700000021002100340035003400000000000000000000000000000000000000000000002100210036003400330037003700370037003700370037003700370037003700210021002100220021002100210021002100210022002100210021002100210021";
        mapCols = 16;
        mapRows = 16;

        tileWidth = 32;
        tileHeight = 32;

        if (debug_mode)
        {
            gameViewWidth = 6 * tileWidth;
            gameViewHeight = 6 * tileHeight;

            gameViewRow = 2;
            gameViewCol = 2;
        }
        else
        {
            gameViewWidth = 10 * tileWidth;
            gameViewHeight = 10 * tileHeight;

            gameViewRow = 0;
            gameViewCol = 0;
        }

        gameViewOffsetX = gameViewRow * tileWidth;
        gameViewOffsetY = gameViewCol * tileHeight;

        gameViewRows = gameViewHeight / tileHeight;
        gameViewCols = gameViewWidth / tileWidth;

        boardViewRow = 0;
        boardViewCol = 0;

        playerScrollX = 0;
        playerScrollY = 0;

        playerX = 1 * tileWidth;
        playerY = 1 * tileHeight;

        scrollHorizontalHigh = (gameViewCols - 2) * tileWidth;
        scrollHorizontalLow = 1 * tileWidth;

        scrollVerticalHigh = (gameViewRows - 2) * tileHeight;
        scrollVerticalLow = 1 * tileHeight;

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
                char tileStr[5];
                for (int ii = 0; ii < 4; ii++)
                {
                    tileStr[ii] = mazeMapAsString[idx + ii];
                }
                tileStr[4] = '\0';
                // single char number to int
                // mazeMap[i][j] = mazeMapAsString[] - '0'; 
           
                // 4-char number to int is better
                mazeMap[i][j] = std::stoi(tileStr); 
                idx += 4;
            }
        }

        // example graphics from opengameart

        bgTilesTexture = loadTextureFile("tiles.png");
        bgTextureSize = bgTilesTexture->getSize();

        playerTexture = loadTextureFile("player.png");
        player = new Player(playerTexture);
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
                    case sf::Keyboard::X:
                        input.keyX = 1;
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
                    case sf::Keyboard::X:
                        input.keyX = 0;
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

    bool is_collision(int player_x, int player_y)
    {
        int collided = 0;
        
        for (int i = 0; i < gameViewRows + 2; ++i)
        {
            for (int j = 0; j < gameViewCols + 2; ++j)
            {
                if ((boardViewRow - 1 + i >= 0) and (boardViewCol - 1 + j >= 0) and (boardViewCol - 1 + j < mapCols) // if mapCols is 32 then max j is 31 !!!
                    and (boardViewRow - 1 + i < mapRows))
                {
                    if (mazeMap[boardViewRow - 1 + i][boardViewCol - 1 + j] > 0)
                    {
                        collided = wallCollided(player_x, player_y, player->sizeX, player->sizeY, (j * tileWidth) - playerScrollX - tileWidth, (i * tileHeight) - playerScrollY - tileWidth, tileWidth, tileHeight);
    
                        if (collided) {
                            return 1;
                        }
                    }
                }
            }
        }
        return 0;
    }

    bool canGoLeft()
    {
        int screenPlayerX = player->posX;
        int screenPlayerY = player->posY;

        bool collided = 0;

        collided = is_collision(screenPlayerX-1, screenPlayerY);

         if (collided)
        {
            return 0;
        }
        return 1;
    }

    bool canGoRight()
    {
        int screenPlayerX = player->posX;
        int screenPlayerY = player->posY;

        bool collided = 0;

        collided = is_collision(screenPlayerX+1, screenPlayerY);

        if (collided)
        {
            return 0;
        }
        return 1;
    }

    bool canGoDown()
    {
        bool collided = 0;

        collided = is_collision(player->posX, player->posY+1);

        if (collided)
        {
            return 0;
        }
        return 1;
    }

    bool canGoUp()
    {
        bool collided = 0;

        collided = is_collision(player->posX, player->posY-1);

        if (collided)
        {
            return 0;
        }
        return 1;
    }

    bool wallCollided(int objAx, int objAy, int objAw, int objAh, int objBx, int objBy, int objBw, int objBh)
    {
        if (debug_mode)
        {
            renderTileRed(objAx, objAy, objAw, objAh);
            renderTileGreen(objBx, objBy, objBw, objBh);
        }

        if (objAx < objBx + objBw &&
            objAx + objAw > objBx &&
            objAy < objBy + objBh &&
            objAy + objAh > objBy)
        {
            if (debug_mode)
            {
                // std::cout << "\n"; 
                // std::cout << "A is at " << objAx << " " << objAy << " => [" << objAw << "][" << objAh << "]" << std::endl;
                // std::cout << "B is at " << objBx << " " << objBy << " => [" << objBw << "][" << objBh << "]" << std::endl;
                // std::cout << (objAx < objBx + objBw) << std::endl;
                // std::cout << (objAx + objAw > objBx) << std::endl;
                // std::cout << (objAy < objBy + objBh) << std::endl;
                // std::cout << (objAy + objAh > objBy) << std::endl;
            }
            return true;
        }
        return false;
    }

    void Update()
    {
        int playerSpeed = 1;

        // MOVE PLAYER

        player->dirX = 0;
        player->dirY = 0;

        if (input.moveX > 0)
        {
            player->dirX = 1;

            if (canGoRight())
            {
                playerX = playerX + playerSpeed;

                if (playerX > scrollHorizontalHigh) // reached x max checkpoint
                {
                    if (boardViewCol == mapCols - gameViewCols)
                    {
                        // it means the player can't scroll the map any further and playerX++ is ok here

                        if (playerScrollX < 0)
                        { // but if screen was partially scrolled left, we must rewind

                            playerScrollX = playerScrollX + playerSpeed;

                            playerX = scrollHorizontalHigh; // and then we need to keep player still so it doesnt look like he moves faster
                        }
                    }
                    else
                    {
                        playerX = scrollHorizontalHigh;

                        playerScrollX = playerScrollX + playerSpeed; // scroll right and keep player at max checkpoint pos

                        if (playerScrollX >= tileWidth) // 1 col scrolled
                        {
                            if (boardViewCol < mapCols - gameViewCols) // must be < as 16-6=10 means 10 is last possible boardViewCol
                            {
                                boardViewCol++; // move board viewport +1
                            }

                            playerScrollX = 0; // reset scroll
                        }
                    }
                }
            }
        }
        else if (input.moveX < 0)
        {
            player->dirX = -1;
        
            if (canGoLeft())
            {
                playerX = playerX - playerSpeed;

                if (playerX < scrollHorizontalLow) // reached x min checkpoint
                {
                    if (boardViewCol == 0)
                    {
                        // it means the player can't scroll the map any further and playerX-- is ok here

                        if (playerScrollX > 0)
                        { // but if screen was partially scrolled right, we must rewind
                            playerScrollX = playerScrollX - playerSpeed;
                            playerX = scrollHorizontalLow;
                        }
                    }
                    else
                    {
                        playerX = scrollHorizontalLow;

                        // scroll left and keep player at min checkpoint pos
                        playerScrollX = playerScrollX - playerSpeed;

                        if (abs(playerScrollX) >= tileWidth) // 1 col scrolled
                        {
                            if (boardViewCol > 0) // must be > as 0 is lowest possible boardViewCol
                            {
                                boardViewCol--; // move board viewport -1
                            }

                            playerScrollX = 0; // reset scroll
                        }
                    }
                }
            }
        }

        if (input.moveY > 0)
        {
            if (canGoDown())
            {
                playerY = playerY + playerSpeed;
            }

            player->dirY = 1;

            if (playerY > scrollVerticalHigh)
            {
                if (boardViewRow == mapRows - gameViewRows)
                {
                    if (playerScrollY < 0)
                    {
                        playerScrollY = playerScrollY + playerSpeed;
                        playerY = scrollVerticalHigh;
                    }
                }
                else
                {
                    playerY = scrollVerticalHigh;

                    playerScrollY = playerScrollY + playerSpeed;

                    if (playerScrollY >= tileWidth)
                    {
                        if (boardViewRow < mapRows - gameViewRows)
                        {
                            boardViewRow++;
                        }

                        playerScrollY = 0;
                    }
                }
            }
        }
        else if (input.moveY < 0)
        {
            if (canGoUp())
            {
                playerY = playerY - playerSpeed;
            }

            player->dirY = -1;

            if (playerY < scrollVerticalLow) // reached y min checkpoint
            {
                if (boardViewRow == 0)
                {
                    // it means the player can't scroll the map any further and playerY-- is ok here

                    if (playerScrollY > 0)
                    { // but if screen was partially scrolled up, we must rewind
                        playerScrollY = playerScrollY - playerSpeed;
                        playerY = scrollVerticalLow;
                    }
                }
                else
                {
                    playerY = scrollVerticalLow;

                    playerScrollY = playerScrollY - playerSpeed; // scroll down and keep player at min checkpoint pos

                    if (abs(playerScrollY) == tileHeight) // 1 row scrolled
                    {
                        if (boardViewRow > 0) // must be > as 0 is lowest possible boardViewRow
                        {
                            boardViewRow--; // move board viewport -1
                        }

                        playerScrollY = 0; // reset Y scroll
                    }
                }
            }
        }

        player->posX = playerX;
        player->posY = playerY;

        player->Update();

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

        if (input.keyX)
        {
            if (debug_mode)
            {
                debug_mode = 0;
            }
            else
            {
                debug_mode = 1;
            }
            input.keyX = 0;
        }
    }

    void Render()
    {
        window.clear(Color(0, 0, 0));

        renderMazeMap();

        renderGameViewFrame();

        // renderCompleteMazeMapWithoutAnyOffsets();

        renderPlayer();

        if (debug_mode) {
            renderDebugGrid();

            canGoRight();
            canGoLeft();
        }

        window.display();
    }

    void renderPlayer()
    {
        // if (debug_mode) {
        // sf::RectangleShape tile;
        // tile.setSize(sf::Vector2f(player->sizeX, player->sizeY));
        // tile.setFillColor(sf::Color(200, 150, 0));
        // tile.setPosition(gameViewOffsetX + playerX, gameViewOffsetY + playerY);
        // window.draw(tile);
        // }

        // doesnt matter now / see view rows and border frame
        // player->posX = player->posX + gameViewOffsetX;// - 32;
        // player->posY = player->posY + gameViewOffsetY;// - 32;

        player->Render(window);
    }

    void renderTile(int x, int y, int w, int h)
    {
        sf::RectangleShape tile;
        tile.setSize(sf::Vector2f(w, h));
        tile.setFillColor(sf::Color(0, 0, 220));
        tile.setPosition(gameViewOffsetX + x, gameViewOffsetY + y);
        window.draw(tile);
    }

    void renderTileRed(int x, int y, int w, int h)
    {
        sf::RectangleShape tile;
        tile.setSize(sf::Vector2f(w, h));
        tile.setFillColor(sf::Color(220, 0, 0));
        tile.setPosition(x, y);
        window.draw(tile);
    }

    void renderTileGreen(int x, int y, int w, int h)
    {
        sf::RectangleShape tile;
        tile.setSize(sf::Vector2f(w, h));
        tile.setFillColor(sf::Color(0, 220, 0));
        tile.setPosition(x, y);
        window.draw(tile);
    }

    void renderGameViewFrame()
    {
        sf::RectangleShape rectangle(sf::Vector2f(gameViewWidth, gameViewHeight));
        rectangle.setPosition(gameViewOffsetX, gameViewOffsetY);
        rectangle.setFillColor(sf::Color::Transparent);
        rectangle.setOutlineThickness(1);
        rectangle.setOutlineColor(sf::Color(150, 0, 0));
        window.draw(rectangle);
    }

    void renderMazeMap()
    {
        // std::cout << "boardViewCol: " << boardViewCol << " playerX: " << playerX << " playerScrollX: " << playerScrollX << std::endl;

        if (debug_mode)
        {
            // shows +1 extra row/col outside gameView box
            for (int i = 0; i < gameViewRows + 2; ++i)
            {
                for (int j = 0; j < gameViewCols + 2; ++j)
                {
                    if ((boardViewRow - 1 + i >= 0) and (boardViewCol - 1 + j >= 0) and (boardViewCol - 1 + j < mapCols) // if mapCols is 32 then max j is 31 !!!
                        and (boardViewRow - 1 + i < mapRows))
                    {
                        if (mazeMap[boardViewRow - 1 + i][boardViewCol - 1 + j] > 0)
                        {
                            renderTile(
                                (j * tileWidth) - playerScrollX - tileWidth, // shows -1 col
                                (i * tileHeight) - playerScrollY - tileWidth,
                                tileWidth, tileHeight);
                        }
                    }
                }
            }

            if (0) // debug
            {
                // top
                sf::RectangleShape tile;
                tile.setSize(sf::Vector2f(SCREEN_WIDTH, tileHeight * 2));
                tile.setFillColor(sf::Color(0, 0, 150));
                tile.setPosition(0, 0);
                window.draw(tile);

                // left
                tile.setSize(sf::Vector2f(tileHeight * 2, SCREEN_HEIGHT));
                tile.setFillColor(sf::Color(0, 0, 150));
                tile.setPosition(0, tileHeight * 2);
                window.draw(tile);

                // right
                tile.setSize(sf::Vector2f(tileHeight * 2, SCREEN_HEIGHT));
                tile.setFillColor(sf::Color(0, 0, 150));
                tile.setPosition(tileHeight * 8, tileHeight * 2);
                window.draw(tile);

                // bottom
                tile.setSize(sf::Vector2f(SCREEN_WIDTH, tileHeight * 2));
                tile.setFillColor(sf::Color(0, 0, 150));
                tile.setPosition(0, tileHeight * 8);
                window.draw(tile);
            }
        }
        else
        {
            // shows +1 extra row/col outside gameView box
            for (int i = 0; i < gameViewRows + 2; ++i)
            {
                for (int j = 0; j < gameViewCols + 2; ++j)
                {
                    if ((boardViewRow - 1 + i >= 0) and (boardViewCol - 1 + j >= 0) and (boardViewCol - 1 + j < mapCols) // if mapCols is 32 then max j is 31 !!!
                        and (boardViewRow - 1 + i < mapRows))
                    {
                        if (mazeMap[boardViewRow - 1 + i][boardViewCol - 1 + j] > 0)
                        {
                            renderTexturedTile(
                                (j * tileWidth) - playerScrollX - tileWidth, // shows -1 col
                                (i * tileHeight) - playerScrollY - tileWidth,
                                tileWidth, tileHeight, mazeMap[boardViewRow - 1 + i][boardViewCol - 1 + j]);
                        }
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
                    // bug here as renderTile uses gameViewOffsetX anyway
                    renderTile(j * tileWidth, i * tileHeight, tileWidth, tileHeight);
                }
            }
        }
    }

    void renderDebugGrid() 
    {
        int tileSize = 32;
        sf::Color gridColor(100, 100, 100);

        for (int i = 0; i < gameViewCols + 1; ++i) {
            sf::Vertex line[] =
            {
                sf::Vertex(sf::Vector2f(i * tileSize, 0)),
                sf::Vertex(sf::Vector2f(i * tileSize, (gameViewRows + 1) * tileSize))
            };
            line[0].color = gridColor;
            line[1].color = gridColor;
            window.draw(line, 2, sf::Lines);
        }

        for (int j = 0; j < gameViewRows + 1; ++j) {
            sf::Vertex line[] =
            {
                sf::Vertex(sf::Vector2f(0, j * tileSize)),
                sf::Vertex(sf::Vector2f((gameViewCols + 1) * tileSize, j * tileSize))
            };
            line[0].color = gridColor;
            line[1].color = gridColor;
            window.draw(line, 2, sf::Lines);
        }
    }

    void renderTexturedTile(int x, int y, int w, int h, int tile_index)
    {
        tile_index--; // as 1 is actially index 0 in tiles array
        int bgCols = bgTextureSize.x / 32; // TODO 64 is tile size in texture file, not in world map
        int row_index = tile_index / bgCols;
        int col_index = tile_index % bgCols;
        bgSprite.setTexture(*bgTilesTexture);
        frameSelection = IntRect(col_index * w, row_index * h, w, h);
        bgSprite.setTextureRect(frameSelection);
        bgSprite.setPosition(gameViewOffsetX + x, gameViewOffsetY + y);
        window.draw(bgSprite);
    }

    Texture *loadTextureFile(const char *filename, sf::Color transparencyColor = sf::Color::Transparent)
    {
        Texture *tex = new Texture();
        Image img;
        if (img.loadFromFile(filename))
        {
            img.createMaskFromColor(transparencyColor);
            tex->loadFromImage(img);
        }
        return tex;
    }

    ~AmazeManGame()
    {
        int n = 4;
        for (int i = 0; i < n; ++i)
        {
            delete[] mazeMap[i];
        }
        delete[] mazeMap;
        
        delete bgTilesTexture;
        delete player;
        delete playerTexture;
    }
};

int main()
{
    AmazeManGame game;
    game.Start();
    return 0;
}