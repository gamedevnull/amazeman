#pragma once
#include "Animation.hpp"
#include "GameObject.hpp"

class Player : public GameObject
{
public:
    int state;
    enum stateID {
        GoingLeft,
        GoingRight,
        GoingUp,
        GoingDown,
        Idle
    };
    Animation *animation;
    std::vector<Animation *> animations;

    Player(Texture *texture);
    void Update();
    void Render(sf::RenderWindow &window);
    ~Player();
};