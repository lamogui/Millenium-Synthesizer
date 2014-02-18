#include "player.hpp"

Player::Player() : _head(1.0)
{
  _head.setFillColor(sf::Color(0,0,0,255));
  _head.setOrigin(_head.getRadius(),_head.getRadius());
}

Player::~Player()
{
  
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
  target.draw(_head,states);
  
}

void Player::update(float dt)
{
  _head.setPosition(getPosition());
}