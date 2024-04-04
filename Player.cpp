#include "include/Player.hpp"
#include "include/Animation.hpp"
#include <iostream>

Player::Player(Texture *texture)
{
   GameObject::Init(64, 64, 16, 20);

   animation = new Animation(texture, 64, 64, 7, 0, 0, 5);
   animations.push_back(animation);

   animation = new Animation(texture, 64, 64, 1, 0, 2 * 64, 5);
   animations.push_back(animation);

   animation = new Animation(texture, 64, 64, 7, 0, 2 * 64, 5);
   animations.push_back(animation);

   animation = new Animation(texture, 64, 64, 7, 0, 1 * 64, 5);
   animations.push_back(animation);

   animation = new Animation(texture, 64, 64, 7, 0, 3 * 64, 5);
   animations.push_back(animation);
}

void Player::Update()
{
   // TODO do all the magic after debug is done
   // animation->Update();
   // GameObject::Update();
}

void Player::Render(sf::RenderWindow &window)
{
   // GameObject::Render(window);

   Sprite sprite;

   if (dirY > 0)
   {
      animations[2]->Update();
      sprite = animations[2]->getSprite();
   }
   else if (dirY < 0)
   {
      animations[0]->Update();
      sprite = animations[0]->getSprite();
   }
   else if (dirX > 0)
   {
      animations[4]->Update();
      sprite = animations[4]->getSprite();
   }
   else if (dirX < 0)
   {
      animations[3]->Update();
      sprite = animations[3]->getSprite();
   }
   else
   {
      sprite = animations[1]->getSprite();
   }

   // fix scale and position
   sprite.setScale(0.5f, 0.5f);
   sprite.setPosition(posX - 5, posY - 10);

   window.draw(sprite);
}

Player::~Player()
{
   // TODO delete animations[];
}