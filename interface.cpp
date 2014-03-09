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
    _knob_sprite.setColor(sf::Color(242,42,42,255));
    
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


ScrollBar::ScrollBar(sf::View& view, int zone,bool h):
_view(&view),
_bar(),
_catch(0),
_current_offset(0),
_zone_size(zone),
_horizontal(h)
{
  _bar.setFillColor(sf::Color(242,42,42,140));
  if (_horizontal) {
    _current_offset = _view->getCenter().x-view.getSize().x/2;
    
  }
  else {
    _current_offset = _view->getCenter().y-view.getSize().y/2;
  }
  update();
}

ScrollBar::~ScrollBar() {
  
}

bool ScrollBar::onMousePress(float x, float y)
{
  if (_horizontal && _zone_size <= _view->getSize().x) return false;
  else if (!_horizontal && _zone_size <= _view->getSize().y) return false;
  
  if (_bar.getGlobalBounds().contains(x,y))
  {
    if (_horizontal) _catch=x-_bar.getPosition().x;
    else _catch=y-_bar.getPosition().y;
    _bar.setFillColor(sf::Color(242,42,42,255));
    return true;
  }
  return false;
}

void ScrollBar::onMouseMove(float x, float y){
  if (_horizontal)
  {
    float r_x=x-_catch-_view->getCenter().x+_view->getSize().x/2;
    _current_offset = r_x*_zone_size/_view->getSize().x;
    if (_current_offset < 0) _current_offset=0;
    if (_current_offset > _zone_size - _view->getSize().x) _current_offset=_zone_size - _view->getSize().x;
  }
  else
  {
    float r_y=y-_catch-_view->getCenter().y+_view->getSize().y/2;
    _current_offset = r_y*_zone_size/_view->getSize().y;
    if (_current_offset < 0) _current_offset=0;
    if (_current_offset > _zone_size - _view->getSize().y) _current_offset=_zone_size - _view->getSize().y;
  }
  update();
}
void ScrollBar::onMouseRelease(float x, float y)
{
  _bar.setFillColor(sf::Color(242,42,42,140));
}

void ScrollBar::draw (sf::RenderTarget &target, sf::RenderStates states) const
{
  if ((_horizontal && _zone_size > _view->getSize().x) || 
      (!_horizontal && _zone_size > _view->getSize().y))
    target.draw(_bar);
}

void ScrollBar::update()
{
  if ((_horizontal && _zone_size <= _view->getSize().x) || 
      (!_horizontal && _zone_size <= _view->getSize().y))
  {
    _current_offset=0;
    //_view->setCenter(_view->getSize().x/2.f, _view->getSize().y/2.f);
  }

  else if (_horizontal) {
    _bar.setSize(sf::Vector2f(_view->getSize().x*_view->getSize().x/_zone_size,12));
    _view->setCenter(_current_offset+_view->getSize().x/2,_view->getCenter().y);
    _bar.setPosition(_current_offset + _current_offset*_view->getSize().x/(float)_zone_size,
                      _view->getCenter().y+_view->getSize().y/2-12);
  }
  else
  {
    _bar.setSize(sf::Vector2f(12,_view->getSize().y*_view->getSize().y/_zone_size));
    _view->setCenter(_view->getCenter().x,_current_offset+_view->getSize().y/2);
    _bar.setPosition(_view->getCenter().x+_view->getSize().x/2-12,
                      _current_offset + _current_offset*_view->getSize().y/(float)_zone_size);
  }              
}


Interface::Interface(const sf::Vector2i& zone,const sf::Vector2f &size):
_mouseCatcher(),
_drawables(),
_view(size/2.f,size),
_zone(zone),
_verticalBar(_view,_zone.y,false),
_horizontalBar(_view,_zone.x,true)
{
  addMouseCatcher(&_verticalBar);
  addMouseCatcher(&_horizontalBar);
}

Interface::~Interface()
{
  
}

MouseCatcher* Interface::onMousePress(float x, float y)
{
  for (unsigned int i=0; i < _mouseCatcher.size();i++)
  {
    if (_mouseCatcher[i]->onMousePress(x,y)) 
      return _mouseCatcher[i];
  }
}

void Interface::setViewSize(float x, float y)
{
  _view.setSize(x,y);
  _verticalBar.update();
  _horizontalBar.update();
}

void Interface::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  //premier ajouté dernier dessiné
  for (unsigned int i=_drawables.size(); i ;)
  {
    target.draw(*_drawables[--i]);
  }
}

void Interface::update()
{
  for (unsigned int i=0; i < _mouseCatcher.size();i++)
  {
    _mouseCatcher[i]->update(); 
  }
}

void Interface::addMouseCatcher(MouseCatcher* c)
{
  _mouseCatcher.push_back(c);
  _drawables.push_back(c);
}

void Interface::addDrawable(sf::Drawable* c)
{
   _drawables.push_back(c);
}

