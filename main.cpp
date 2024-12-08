#include <iostream>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace sf;

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 320;

const int OBJECT_ID_PLAYER = 1;
const int OBJECT_ID_ENEMY = 2;

const int SCROLL_BOUND = 224;

enum class GameState
{
  StartScreen,
  Playing,
  GameOver
};

class AIComponent
{
public:
  int dirX, dirY;
  void RandomizeMoveDirection()
  {
    dirX = (rand() % 3) - 1;
    dirY = (rand() % 3) - 1;
    if (dirX == 0 && dirY == 0)
    {
      RandomizeMoveDirection();
    }
  }
};

class Animation
{
public:
  sf::Texture *texture;
  sf::IntRect frameSelection;
  int frameWidth, frameHeight, framesCount;
  float delay, timer;

  Animation(sf::Texture *texture, int frameWidth, int frameHeight, int framesCount, int firstFrameX, int firstFrameY, float animationDelay)
      : texture(texture), frameWidth(frameWidth), frameHeight(frameHeight), framesCount(framesCount), delay(animationDelay), timer(0)
  {
    frameSelection = sf::IntRect(firstFrameX, firstFrameY, frameWidth, frameHeight);
  }

  void Update(float deltaTime)
  {
    timer += deltaTime;
    if (timer > delay)
    {
      nextFrame();
      timer = 0;
    }
  }

  void applyToSprite(sf::Sprite &sprite)
  {
    sprite.setTexture(*texture);
    sprite.setTextureRect(frameSelection);
  }

private:
  void nextFrame()
  {
    frameSelection.left = (frameSelection.left + frameWidth) % (frameWidth * framesCount);
  }
};

class AnimationComponent
{
public:
  void addAnimation(int state, Animation *animation)
  {
    animations[state] = animation;
  }

  void setAnimation(int state)
  {
    if (currentState != state && animations.count(state))
    {
      currentState = state;
      currentAnimation = animations[state];
    }
  }

  void Update(float deltaTime, sf::Sprite &sprite)
  {
    if (currentAnimation)
    {
      currentAnimation->Update(deltaTime);
      currentAnimation->applyToSprite(sprite);
    }
  }

private:
  std::map<int, Animation *> animations;
  Animation *currentAnimation = nullptr;
  int currentState = -1;
};

class PositionComponent
{
public:
  float x, y;
  PositionComponent(float x = 0, float y = 0) : x(x), y(y) {}
};

class OffsetComponent
{
public:
  int x, y;
  OffsetComponent(int x = 0, int y = 0) : x(x), y(y) {}
};

class SizeComponent
{
public:
  int w, h;
  SizeComponent(int w = 0, int h = 0) : w(w), h(h) {}
};

class StateComponent
{
public:
  enum BaseState
  {
    Idle = 0
  };

  virtual void setState(int newState)
  {
    if (currentState != newState)
    {
      if (onStateChange)
      {
        onStateChange(currentState, newState);
      }
      previousState = currentState;
      currentState = newState;
    }
  }

  int getState() const
  {
    return currentState;
  }

  int getPreviousState() const
  {
    return previousState;
  }

  void setStateChangeCallback(std::function<void(int, int)> callback)
  {
    onStateChange = callback;
  }

protected:
  int currentState = Idle;
  int previousState = -1;
  std::function<void(int, int)> onStateChange = nullptr;
};

class PlayerStateComponent : public StateComponent
{
public:
  enum PlayerState
  {
    GoingUp = 1,
    GoingDown,
    GoingLeft,
    GoingRight
  };

  void setState(int newState) override
  {
    if (isValidState(newState))
    {
      StateComponent::setState(newState);
    }
  }

private:
  bool isValidState(int state)
  {
    return state == Idle ||
           state == GoingUp ||
           state == GoingDown ||
           state == GoingLeft ||
           state == GoingRight;
  }
};

class GameMap
{
public:
  int mapCols, mapRows;
  int **mapArray;
  int mapViewCols, mapViewRows;
  int tileWidth, tileHeight;
  OffsetComponent *scroll;
  Texture *mapTexture;

  GameMap(const char *data, int cols, int rows, OffsetComponent *offset, Texture *texture)
  {
    mapCols = cols;
    mapRows = rows;

    mapViewCols = 10;
    mapViewRows = 10;

    tileWidth = 32;
    tileHeight = 32;

    scroll = offset;

    mapTexture = texture;

    initMap();
    fillMap(data);
  }

  void initMap()
  {
    mapArray = new int *[mapRows];
    for (int i = 0; i < mapRows; ++i)
    {
      mapArray[i] = new int[mapCols];
    }
  }

  void fillMap(const char *mapData)
  {
    int idx = 0;
    for (int i = 0; i < mapRows; ++i)
    {
      for (int j = 0; j < mapCols; ++j)
      {
        char tileStr[5];
        for (int ii = 0; ii < 4; ii++)
        {
          tileStr[ii] = mapData[idx + ii];
        }
        tileStr[4] = '\0';

        mapArray[i][j] = std::stoi(tileStr);
        idx += 4;
      }
    }
  }

  bool IsWalkable(int newX, int newY, int buffer = 0, int playerSizeX = 20, int playerSizeY = 20, int tileSize = 32)
  {
    int mapWidth = mapCols;
    int mapHeight = mapRows;

    int topLeftX = static_cast<int>(newX - buffer) / tileSize;
    int topLeftY = static_cast<int>(newY - buffer) / tileSize;

    int bottomRightX = static_cast<int>(newX + playerSizeX + buffer - 1) / tileSize;
    int bottomRightY = static_cast<int>(newY + playerSizeY + buffer - 1) / tileSize;

    for (int x = topLeftX; x <= bottomRightX; ++x)
    {
      for (int y = topLeftY; y <= bottomRightY; ++y)
      {
        if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        {
          return false;
        }

        if (mapArray[y][x] > 0)
        {
          return false;
        }
      }
    }

    return true;
  }

  int getMaxScrollX()
  {
    return (mapCols * tileWidth) - (mapViewCols * tileWidth);
  }

  int getMaxScrollY()
  {
    return (mapRows * tileHeight) - (mapViewRows * tileHeight);
  }

  void Render(sf::RenderWindow &window)
  {
    drawMap(window);
  }

  void drawMap(sf::RenderWindow &window)
  {
    int scrollX = scroll->x;
    int scrollY = scroll->y;

    int mapOffsetCol = scrollX / tileWidth;
    int mapOffsetRow = scrollY / tileHeight;

    int offset_scroll_x = scrollX % tileWidth;
    int offset_scroll_y = scrollY % tileHeight;

    int extraRowCol = 2; // +1 extra row/col outside gameView box (left + right = +2)

    for (int i = 0; i < mapViewRows + extraRowCol; ++i)
    {
      for (int j = 0; j < mapViewCols + extraRowCol; ++j)
      {
        if (
            (mapOffsetRow - 1 + i >= 0) and
            (mapOffsetCol - 1 + j >= 0) and
            (mapOffsetCol - 1 + j < mapCols) and // if mapCols is 32 then max j is 31 !!! TODO really?
            (mapOffsetRow - 1 + i < mapRows))
        {
          if (mapArray[mapOffsetRow - 1 + i][mapOffsetCol - 1 + j] > 0)
          {
            drawTexturedTile(
                (j * tileWidth) - offset_scroll_x - tileWidth, // shows -1 col TODO what?
                (i * tileHeight) - offset_scroll_y - tileWidth,
                mapArray[mapOffsetRow - 1 + i][mapOffsetCol - 1 + j],
                window);
          }
        }
      }
    }
  }

  void drawTile(int x, int y, int tile_index, sf::RenderWindow &window)
  {
    sf::RectangleShape tile;
    tile.setSize(sf::Vector2f(tileWidth, tileHeight));
    tile.setFillColor(sf::Color(0, 0, 255));
    tile.setPosition(x, y);
    window.draw(tile);
  }

  void drawTexturedTile(int x, int y, int tile_index, sf::RenderWindow &window)
  {
    tile_index--; // as 1 is actially index 0 in tiles array

    Vector2u bgTextureSize;
    bgTextureSize = mapTexture->getSize();

    int bgCols = bgTextureSize.x / tileWidth; // TODO as for tileWidth -> is tile size in texture file always equals tile size in world map?
    int bgRow = tile_index / bgCols;
    int bgCol = tile_index % bgCols;

    Sprite bgSprite;
    IntRect frameSelection;

    bgSprite.setTexture(*mapTexture);
    frameSelection = IntRect(bgCol * tileWidth, bgRow * tileHeight, tileWidth, tileHeight);

    bgSprite.setTextureRect(frameSelection);
    bgSprite.setPosition(x, y);

    window.draw(bgSprite);
  }
};

class MoveComponent
{
public:
  PositionComponent &position;
  GameMap *map;
  int objSizeX, objSizeY;
  float objSpeed = 1;

  MoveComponent(PositionComponent &pos, GameMap *map = nullptr, int objSizeX = 0, int objSizeY = 0)
      : position(pos), map(map), objSizeX(objSizeX), objSizeY(objSizeY) {}

  void SetMap(GameMap *newMap)
  {
    map = newMap;
  }

  bool Move(float dx, float dy, float deltaTime)
  {
    float moveX = dx * objSpeed * deltaTime;
    float moveY = dy * objSpeed * deltaTime;

    if (map && map->IsWalkable(static_cast<int>(position.x + moveX), static_cast<int>(position.y + moveY), 0, objSizeX, objSizeY))
    {
      position.x += moveX;
      position.y += moveY;
      return true;
    }
    return false;
  }
};

class GameObject
{
public:
  PositionComponent *position;
  OffsetComponent *offset;
  SizeComponent *size;
  MoveComponent *moveComponent;
  int objId;

  GameObject(OffsetComponent *offset) : offset(offset)
  {
    position = new PositionComponent();
    size = new SizeComponent();
    size->w = 20;
    size->h = 20;

    moveComponent = nullptr;
  }

  void Init(int posX, int posY, int id, int sizeX = 20, int sizeY = 20)
  {
    position->x = posX;
    position->y = posY;
    size->w = sizeX;
    size->h = sizeY;
    objId = id;
    moveComponent = new MoveComponent(*position, nullptr, size->w, size->h);
    moveComponent->objSpeed = 1;
  }

  virtual void Update(float deltaTime) {}

  virtual void Render(sf::RenderWindow &window)
  {
    sf::RectangleShape tile;
    tile.setSize(sf::Vector2f(size->w, size->h));
    tile.setFillColor(sf::Color(0, 0, 255));
    tile.setPosition(position->x - offset->x, position->y - offset->y);
    window.draw(tile);
  }

  bool Collided(GameObject *otherObject)
  {
    if (position->x < otherObject->position->x + otherObject->size->w &&
        position->x + size->w > otherObject->position->x &&
        position->y < otherObject->position->y + otherObject->size->h &&
        position->y + size->h > otherObject->position->y)
    {
      return true;
    }
    return false;
  }
};

class GameInput
{
public:
  int moveX, moveY;
  bool keyW, keyS, keyA, keyD, keyX;
  GameInput() : moveX(0), moveY(0), keyW(0), keyS(0), keyA(0), keyD(0), keyX(0) {}
};

class PlayerControllerComponent
{
public:
  GameInput &input;
  PlayerStateComponent *stateComponent;
  PlayerControllerComponent(GameInput &input) : input(input)
  {
    stateComponent = nullptr;
  }

  void setStateComponent(PlayerStateComponent *stateComponent1)
  {
    stateComponent = stateComponent1;
  }

  void Update(MoveComponent *moveComponent, OffsetComponent *offset, float deltaTime)
  {

    stateComponent->setState(PlayerStateComponent::Idle);

    if (input.moveX > 0)
    {
      stateComponent->setState(PlayerStateComponent::GoingRight);
      moveComponent->Move(1, 0, deltaTime);
      if ((offset->x < moveComponent->map->getMaxScrollX()) and (moveComponent->position.x > (offset->x + SCROLL_BOUND)) and ((offset->x + SCREEN_WIDTH) < (moveComponent->map->mapCols * moveComponent->map->tileWidth)))
      {
        offset->x++;
      }
    }

    int playersizeX = 20;
    int playersizeY = 20;

    if (input.moveX < 0)
    {
      moveComponent->Move(-1, 0, deltaTime);
      stateComponent->setState(PlayerStateComponent::GoingLeft);
      if (offset->x > 0 and moveComponent->position.x < (offset->x + SCREEN_WIDTH - SCROLL_BOUND - playersizeX) and offset->x > 0)
      {
        offset->x--;
      }
    }

    if (input.moveY < 0)
    {
      moveComponent->Move(0, -1, deltaTime);
      stateComponent->setState(PlayerStateComponent::GoingUp);
      if (offset->y > 0 and moveComponent->position.y < (offset->y + SCREEN_HEIGHT - SCROLL_BOUND - playersizeY) and offset->y > 0)
      {
        offset->y--;
      }
    }

    if (input.moveY > 0)
    {
      moveComponent->Move(0, 1, deltaTime);
      stateComponent->setState(PlayerStateComponent::GoingDown);
      if (offset->y < moveComponent->map->getMaxScrollY() and moveComponent->position.y > (offset->y + SCROLL_BOUND) and (offset->y + SCREEN_HEIGHT) < (moveComponent->map->mapRows * moveComponent->map->tileHeight))
      {
        offset->y++;
      }
    }
  }
};

class Player : public GameObject
{
public:
  PlayerControllerComponent *playerController;
  sf::Sprite sprite;
  AnimationComponent *animationComponent;
  PlayerStateComponent *stateComponent;

  Player(OffsetComponent *offset, PlayerControllerComponent *controller, sf::Texture *texture) : GameObject(offset)
  {

    sprite.setTexture(*texture);
    stateComponent = new PlayerStateComponent();

    animationComponent = new AnimationComponent();
    controller->setStateComponent(stateComponent);

    animationComponent->addAnimation(PlayerStateComponent::Idle,
                                     new Animation(texture, 64, 64, 1, 0, 2 * 64, 5));
    animationComponent->addAnimation(PlayerStateComponent::GoingRight,
                                     new Animation(texture, 64, 64, 7, 0, 3 * 64, 5));
    animationComponent->addAnimation(PlayerStateComponent::GoingLeft,
                                     new Animation(texture, 64, 64, 7, 0, 1 * 64, 5));
    animationComponent->addAnimation(PlayerStateComponent::GoingUp,
                                     new Animation(texture, 64, 64, 7, 0, 0, 5));
    animationComponent->addAnimation(PlayerStateComponent::GoingDown,
                                     new Animation(texture, 64, 64, 7, 0, 2 * 64, 5));

    stateComponent->setStateChangeCallback([this](int oldState, int newState)
                                           { animationComponent->setAnimation(newState); });
    animationComponent->setAnimation(PlayerStateComponent::Idle);
    playerController = controller;
  }

  void Update(float deltaTime) override
  {
    playerController->Update(moveComponent, offset, deltaTime);
    animationComponent->Update(1, sprite);
  }

  void Render(sf::RenderWindow &window) override
  {
    // sf::RectangleShape tile;
    // tile.setSize(sf::Vector2f(size->w, size->h));
    // tile.setFillColor(sf::Color(0, 255, 0));
    // tile.setPosition(position->x - offset->x, position->y - offset->y);
    // window.draw(tile);
    sprite.setScale(0.5f, 0.5f);
    sprite.setPosition(position->x - offset->x - 10, position->y - offset->y - 10);
    window.draw(sprite);
  }
};

class Enemy : public GameObject
{
public:
  sf::Sprite sprite;
  AIComponent aicomponent;

  Enemy(OffsetComponent *offset, sf::Texture *texture) : GameObject(offset)
  {
    sprite.setTexture(*texture);
    sf::IntRect frameSelection = sf::IntRect(0, 0, 32, 32);
    sprite.setTextureRect(frameSelection);
    aicomponent.RandomizeMoveDirection();
  }

  void Update(float deltaTime) override
  {
    if (!moveComponent->Move(aicomponent.dirX, aicomponent.dirY, deltaTime))
    {
      aicomponent.RandomizeMoveDirection();
    }
  }

  void Render(sf::RenderWindow &window) override
  {
    // sf::RectangleShape tile;
    // tile.setSize(sf::Vector2f(size->w, size->h));
    // tile.setFillColor(sf::Color(255, 0, 0));
    // tile.setPosition(position->x - offset->x, position->y - offset->y);
    // window.draw(tile);
    // sprite.setScale(0.5f, 0.5f);
    sprite.setPosition(position->x - offset->x, position->y - offset->y);
    window.draw(sprite);
  }
};

class Game
{
public:
  RenderWindow window;
  GameInput input;

  OffsetComponent *offset;

  GameMap *gameMap;

  PlayerControllerComponent *playerController;

  std::vector<GameObject *> gameObjects;

  Texture *mapTexture;
  Texture *playerTexture;
  Texture *enemyTexture;

  sf::Clock clock;

  GameState currentState = GameState::StartScreen;

  float deltaTime = clock.restart().asSeconds();
  Game() : window(VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Amazeman!")
  {
    srand(static_cast<unsigned>(time(0)));

    window.setFramerateLimit(60);

    mapTexture = loadTextureFile("tiles.png");
    playerTexture = loadTextureFile("jones.png");
    enemyTexture = loadTextureFile("enemy.png");

    offset = new OffsetComponent();

    gameMap = new GameMap("0001002100210021002100290029002100300031002100210021002100210021000100000000000000000000000000000000000000000000000000000000002100010000001700170017001700000017000000130000001300130013000000210001000000170000000000000000001700000013000000000000001300000021000100000017000000170017001700170000001300000000000000130000002100010000001700000017001700170017000000130013001300130013000000210021000000170000001700170017001700000000000000000000000000000021002100000017000000000000000000000000002500260025000000000000002100240000001700000000000000000000000000270000002800000000000000210021000000170000000000000000000000000025000000270000000000000022002100000017001700170017001700170017001700000017001700170000002100210000000000000000000000000000000000000000000000000000000000210022001700170017003700370037003700370037003700370037003700000021002100340035003400000000000000000000000000000000000000000000002100210036003400330037003700370037003700370037003700370037003700210021002100220021002100210021002100210022002100210021002100210021", 16, 16, offset, mapTexture);

    playerController = new PlayerControllerComponent(input);

    Player *player = new Player(offset, playerController, playerTexture);
    player->Init(38, 38, OBJECT_ID_PLAYER, 12, 20);
    player->moveComponent->SetMap(gameMap);
    player->moveComponent->objSpeed = 50;
    AddObject(player);

    Enemy *enemy = new Enemy(offset, enemyTexture);
    enemy->Init(196, 256, OBJECT_ID_ENEMY, 32, 32);
    enemy->moveComponent->SetMap(gameMap);
    enemy->moveComponent->objSpeed = 60;
    AddObject(enemy);

    enemy = new Enemy(offset, enemyTexture);
    enemy->Init(196, 256, OBJECT_ID_ENEMY, 32, 32);
    enemy->moveComponent->SetMap(gameMap);
    enemy->moveComponent->objSpeed = 65;
    AddObject(enemy);

    enemy = new Enemy(offset, enemyTexture);
    enemy->Init(196, 256, OBJECT_ID_ENEMY, 32, 32);
    enemy->moveComponent->SetMap(gameMap);
    enemy->moveComponent->objSpeed = 70;
    AddObject(enemy);
  }

  void restartGame()
  {
    for (auto obj1 : gameObjects)
    {
      if (obj1->objId == OBJECT_ID_PLAYER)
      {
        obj1->position->x = 38;
        obj1->position->y = 38;
        offset->x = 0;
        offset->y = 0;
      }

      if (obj1->objId == OBJECT_ID_ENEMY)
      {
        obj1->position->x = 196;
        obj1->position->y = 256;
      }
    }
  }

  void AddObject(GameObject *obj)
  {
    gameObjects.push_back(obj);
  }

  void Start()
  {
    while (window.isOpen())
    {
      GetEvents();
      Update();
      Render();
    }
  }

  void GetEvents()
  {
    sf::Event e;
    while (window.pollEvent(e))
    {
      switch (e.type)
      {
      case sf::Event::Closed:
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
  }

  void RenderStartScreen()
  {
    sf::Font font;
    font.loadFromFile("retro.ttf");
    sf::Text text("Press X to start", font, 12);
    text.setFillColor(sf::Color::White);
    text.setPosition(SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT / 2);
    window.draw(text);
  }

  void RenderGameOverScreen()
  {
    sf::Font font;
    font.loadFromFile("retro.ttf");
    sf::Text text("Press X to restart", font, 12);
    text.setFillColor(sf::Color::Red);
    text.setPosition(SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT / 2);
    window.draw(text);
  }

  void Render()
  {
    window.clear(Color(30, 30, 30));

    if (currentState == GameState::StartScreen)
    {
      RenderStartScreen();
    }
    else if (currentState == GameState::Playing)
    {

      gameMap->Render(window);

      for (const auto obj : gameObjects)
      {
        obj->Render(window);
      }
    }
    else if (currentState == GameState::GameOver)
    {
      RenderGameOverScreen();
    }

    window.display();
  }

  void Update()
  {

    if (currentState == GameState::Playing)
    {

      bool gameOver = 0;

      for (auto obj1 : gameObjects)
      {
        for (auto obj2 : gameObjects)
        {
          if ((obj1->objId != obj2->objId) && obj1->Collided(obj2))
          {
            gameOver = 1;
            break;
          }
        }
      }

      if (gameOver)
      {
        currentState = GameState::GameOver;
        restartGame();
        return;
      }

      float deltaTime = clock.restart().asSeconds();

      for (auto obj : gameObjects)
      {
        obj->Update(deltaTime);
      }
    }
    else
    {

      if (input.keyX)
      {
        input.keyX = 0;
        if (currentState == GameState::StartScreen)
        {
          currentState = GameState::Playing;
        }
        else if (currentState == GameState::GameOver)
        {
          currentState = GameState::StartScreen;
        }
      }
    }
  }

  Texture *loadTextureFile(const char *filename, sf::Color transparencyColor = sf::Color::Transparent)
  {
    Texture *texture = new Texture();
    Image image;
    if (image.loadFromFile(filename))
    {
      image.createMaskFromColor(transparencyColor);
      texture->loadFromImage(image);
    }
    return texture;
  }
};

int main()
{
  Game game;
  game.Start();
  return 0;
}