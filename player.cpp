#include "player.hpp"

#include <cmath>
#include <iostream>

float get_angle2vec(const sf::Vector2f& u,const sf::Vector2f& v)
{
  const float norm_u = sqrt(u.x*u.x+u.y*u.y);
  const float norm_v = sqrt(v.x*v.x+v.y*v.y);
  const float mycos = (u.x*v.x+v.y*u.y)/(norm_u*norm_v);
  const float mysin = (u.x*v.y-u.y*v.x)/(norm_u*norm_v);
  return atan2(mysin,mycos);
  
}

Player::Player() : _head(0.6),
_eye1(0.1),
_eye2(0.1),
_corpse(sf::Vector2f(2.5,1.2))
{
  _head.setFillColor(sf::Color(0,0,0,255));
  _head.setOrigin(_head.getRadius(),_head.getRadius());
  _corpse.setFillColor(sf::Color(255,0,0,255));
  _corpse.setOrigin(2.5*0.5,1.2*0.5);
  
  _eye1.setFillColor(sf::Color(255,255,255,255));
  _eye1.setOrigin(-0.3,0.3);
  _eye2.setFillColor(sf::Color(255,255,255,255));
  _eye2.setOrigin(0.3,0.3);
}

Player::~Player()
{
  
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
  target.draw(_corpse,states);
  target.draw(_head,states);
  target.draw(_eye1,states);
  target.draw(_eye2,states);
}



void Player::update(float dt)
{
  _velocity+=_acceleration*dt;
  setRotation(180.0*atan2(_velocity.x,-(_velocity.y+0.01))/(3.1415));
  setPosition(getPosition()+_velocity*dt);
  
  
  _corpse.setRotation(getRotation());
  _head.setRotation(getRotation());
  _eye1.setRotation(getRotation());
  _eye2.setRotation(getRotation());
  
  
  _corpse.setPosition(getPosition());
  _head.setPosition(getPosition());
  _eye1.setPosition(getPosition());
  _eye2.setPosition(getPosition());
  
}

void Player::think(const SoccerBall& ball, std::vector<Player*>& up_team, std::vector<Player*>& down_team)
{
  float norm = sqrt((ball.getPosition().x-getPosition().x)*(ball.getPosition().x-getPosition().x)+
	            (ball.getPosition().y-getPosition().y)*(ball.getPosition().y-getPosition().y))+0.01;
  float ball_speed = sqrt((ball.velocity.x)*(ball.velocity.x)+
			  (ball.velocity.y)*(ball.velocity.y))+0.01;
  float speed = sqrt((_velocity.x)*(_velocity.x)+
		     (_velocity.y)*(_velocity.y))+0.01;
		     
  
  float angle=get_angle2vec(_velocity,ball.getPosition()-getPosition());
  float angle_deg=abs(180.0*angle/3.1415);
  
  std::cout << "angle " << angle_deg  << "  " << angle << std::endl;
  
  if (angle_deg > 45.0 && speed > 0.5)
  {
    _acceleration.x = -_velocity.x*6.0/speed;
    _acceleration.y = -_velocity.y*6.0/speed;
  }
  else if (norm > 6.0 && speed < 9.0) //course vers la balle
  {
    
    _acceleration.x = (ball.getPosition().x - getPosition().x)*2.0/norm;
    _acceleration.y = (ball.getPosition().y - getPosition().y)*2.0/norm;
    if (angle_deg > 1.0)
    {
      sf::Vector2f correction_angle(cos(angle),sin(angle));
      std::cout << "correction angle " << correction_angle.x << "  " << correction_angle.y << std::endl;
      //_acceleration-=correction_angle;
    }
  }
  else if (angle_deg < 10.0 && speed > 0.5 && norm < 3.0 && norm > 0.1) //on ralentit on est a 3 mètres
  {
    _acceleration.x = -(ball.getPosition().x - getPosition().x)*speed*norm;
    _acceleration.y = -(ball.getPosition().y - getPosition().y)*speed*norm;
  }
  else
  {
    _acceleration.x=0.0;
    _acceleration.y=0.0;
  }

  
}

