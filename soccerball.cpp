
#include "soccerball.hpp"

SoccerBall::SoccerBall(): sf::CircleShape(0.80),
z(0.0),
velocity(0,0,0),
acceleration(0,0,0),
_frame(0)
{
  _tex.loadFromFile("soccerball.png");
  setOrigin(getRadius()/2.0,getRadius()/2.0);
  setTexture(&_tex);
}

SoccerBall::~SoccerBall()
{
}


void SoccerBall::update(float dt)
{
  _frame++;
  _frame%=25;
  velocity += acceleration*dt;
  z+=velocity.z*dt;
  setPosition(getPosition().x + velocity.x*dt,getPosition().y+ velocity.y*dt);
  setTextureRect(sf::IntRect(_frame*24,0,24,24));
  setScale(z*0.05 + 1.0,z*0.05 + 1.0);
}

bool SoccerBall::intersect(float x, float y)
{
  float x_2 = (x - getPosition().x);
  float y_2 = (y - getPosition().y);
  return x_2*x_2 + y_2*y_2  < getRadius()*getRadius();
}