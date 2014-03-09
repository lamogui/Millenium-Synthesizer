#include "interface.hpp"

#include <iostream>

Knob::Knob(InstrumentParameter* p, const sf::Texture &texture, const sf::IntRect &backRect, const sf::IntRect &knobRect) :
_back_sprite(texture,backRect),
_knob_sprite(texture,knobRect),
_param(p),
_catch_x(0),
_catch_y(0),
_catch_angle(0)
{
  _knob_sprite.setOrigin(knobRect.width/2.f,knobRect.height/2.f);
  _knob_sprite.setPosition(backRect.width/2.f,backRect.height/2.f);
  update();
}

Knob::~Knob()
{
}
    
bool Knob::onMousePress(float x, float y)
{
  if (_param )
  {
    
    sf::Vector2f v(getInverseTransform().transformPoint(x,y));
    _catch_x=v.x;
    _catch_y=v.y;
    
    float x = _catch_x - _knob_sprite.getPosition().x;
    float y = _catch_y - _knob_sprite.getPosition().y;
    float r = _knob_sprite.getOrigin().x;
    x *= x;
    y *= y;
    r *= r;
    
    if (x+y > r) return false;
    
    _catch_angle=_param->getValueToUnsigned(300);
    //std::cout << "catch angle " << _catch_angle << std::endl;
    _knob_sprite.setColor(sf::Color(242,144,144,255));
    
    return true;
  }
  return false;
}

void Knob::onMouseMove(float x, float y)
{
  if (_param )
  {
    sf::Vector2f v(getInverseTransform().transformPoint(x,y));
    float angle = _catch_angle + v.x - _catch_x;
    if (angle < 0) angle = 0;
    else if (angle > 300) angle = 300;
    //std::cout << "Angle " << angle << " catchx " << _catch_x << " x "<< v.x << std::endl;
    _param->setValueFromUnsigned(angle, 300);
    update();
  }
}
void Knob::onMouseRelease(float x, float y)
{
  if ( _param)
    _knob_sprite.setColor(sf::Color(255,255,255,255));
}

void Knob::draw (sf::RenderTarget &target, sf::RenderStates states) const
{
  states.transform *= getTransform();
  target.draw(_back_sprite,states);
  target.draw(_knob_sprite,states);
}

void Knob::update()
{
  if ( _param)
    _knob_sprite.setRotation((float)_param->getValueToUnsigned(300));
}

