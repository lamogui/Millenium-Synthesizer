/*******************************************************
Nom ......... : interface.cpp
Role ........ : Classes de base pour l'interface 
                compatible avec opengl
Auteur ...... : Julien DE LOOR 
Version ..... : V1.7 olol
Licence ..... : © Copydown™
********************************************************/

#include "interface.hpp"

#include <iostream>

#ifdef _MSC_VER
#pragma warning( disable : 4244 )
#pragma warning( disable : 4267 )
#endif

extern sf::Font globalfont; 

Knob::Knob() :
_overColor(242,42,42,255),
_back_sprite(),
_knob_sprite(),
_param(0),
_catched(false),
_catch_x(0),
_catch_y(0),
_catch_angle(0)
{
   
}

Knob::Knob(InstrumentParameter* p, const sf::Texture &texture, 
                                   const sf::IntRect &backRect, 
                                   const sf::IntRect &knobRect) :
_overColor(242,42,42,255),
_back_sprite(texture,backRect),
_knob_sprite(texture,knobRect),
_param(p),
_catched(false),
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

void Knob::setBackTexture(const sf::Texture &texture,const sf::IntRect &rect)
{
   _back_sprite.setTexture(texture);
   _back_sprite.setTextureRect(rect);
}

void Knob::setKnobTexture(const sf::Texture &texture,const sf::IntRect &rect)
{
   _knob_sprite.setTexture(texture);
   _knob_sprite.setTextureRect(rect);
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
    _knob_sprite.setColor(_overColor);
    
    _catched=true;
    return true;
  }
  return false;
}

void Knob::onMouseMove(float x, float y)
{
  if (_param && _catched)
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
  (void)x;
  (void)y;
  if ( _param && _catched)
  {
    _catched=false;
    _knob_sprite.setColor(sf::Color(255,255,255,255));
  }
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
  {
    if (_param->isAuto() && !_catched)
    {
      _knob_sprite.setRotation((float)_param->getAutoToUnsigned(300));
    }
    else
    {
      _knob_sprite.setRotation((float)_param->getValueToUnsigned(300));
    }
  }
}



AbstractButton::AbstractButton(const sf::Vector2f& size, 
                               const sf::String text):
_idleColor(75,75,75,255),
_clickedColor(142,142,142,255),
_shape(size),
_text(),
_idleRect(),
_clickedRect()
{
  _text.setFont(globalfont);
  _text.setCharacterSize(11);
  setText(text);
  _shape.setFillColor(_idleColor);
  _shape.setOutlineThickness(1.f);
  setOutlineColor(sf::Color(255,255,255,255));
}

AbstractButton::AbstractButton( const sf::Texture &texture,
                                const sf::IntRect &idle,
                                const sf::IntRect &clicked) :
_idleColor(255,255,255,255),
_clickedColor(255,255,255,255),
_shape(sf::Vector2f(idle.width,idle.height)),
_idleRect(idle),
_clickedRect(clicked)
{
  _text.setFont(globalfont);
  _text.setCharacterSize(11);
  _shape.setFillColor(_idleColor);
  //_shape.setOutlineThickness(1.f);
  setOutlineColor(sf::Color(255,255,255,255));
  _shape.setTexture(&texture);
  _shape.setTextureRect(_idleRect);
}

AbstractButton::~AbstractButton()
{
}

void AbstractButton::setTexture(const sf::Texture &texture, 
                                const sf::IntRect &idle, 
                                const sf::IntRect &clicked)
{
  _idleRect = idle;
  _clickedRect = clicked;

  _idleColor=sf::Color(255,255,255,255);
  _clickedColor=sf::Color(255,255,255,255);
  _shape.setFillColor(_idleColor);
  _shape.setOutlineThickness(0);
  _shape.setTexture(&texture);
  _shape.setTextureRect(_idleRect);
  /*if (_mode != ButtonMode::toggle)
  {
     if (_catched)
       _shape.setTextureRect(_clickedRect);
     else
       _shape.setTextureRect(_idleRect);
  }*/
}

void AbstractButton::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  states.transform *= getTransform();
  target.draw(_shape,states);
  target.draw(_text,states);
}

void AbstractButton::update()
{
}

void AbstractButton::setText(const sf::String& t)
{
  _text.setString(t);
  _text.setPosition(_shape.getSize().x/2.f - _text.getLocalBounds().width/2.f,
                    _shape.getSize().y/2.f - _text.getLocalBounds().height/2.f);
}

bool AbstractButton::onMousePress(float x, float y)
{

  if (!allowed()) return false;
  sf::Vector2f v(getInverseTransform().transformPoint(x,y));

  float rx = v.x - _shape.getPosition().x;
  float ry = v.y - _shape.getPosition().y;

  if (rx > _shape.getSize().x || rx < 0 ||
      ry > _shape.getSize().y || ry < 0 ) {
    return false;
  }
  _shape.setFillColor(_clickedColor);
  _shape.setTextureRect(_clickedRect);
  return true;
}

void AbstractButton::onMouseMove(float x, float y)
{
  (void)x;(void)y;
}

void AbstractButton::onMouseRelease(float x, float y)
{
   _shape.setFillColor(_idleColor);
   _shape.setTextureRect(_idleRect);
   
   sf::Vector2f v(getInverseTransform().transformPoint(x,y));
    
   float rx = v.x - _shape.getPosition().x;
   float ry = v.y - _shape.getPosition().y;
   if (rx > _shape.getSize().x || rx < 0 ||
       ry > _shape.getSize().y || ry < 0) 
    return;
   else    
    clicked();  
}

ModulableButton::ModulableButton( const sf::Texture &texture,
                                  const sf::IntRect &idle,
                                  const sf::IntRect &clicked,
                                  ButtonMode::Mode mode) :
AbstractButton(texture,idle,clicked),
_mode(mode)
{
  
  
  
}

                     
ModulableButton::ModulableButton( const sf::Vector2f& size, 
                                  const sf::String text,
                                  ButtonMode::Mode mode):
AbstractButton(size,text),
_mode(mode)                                
{
                                   
}

ModulableButton::~ModulableButton() {

}

bool ModulableButton::onMousePress(float x, float y)
{
  if (!allowed()) return false;
  sf::Vector2f v(getInverseTransform().transformPoint(x,y));

   float rx = v.x - _shape.getPosition().x;
   float ry = v.y - _shape.getPosition().y;

   if (rx > _shape.getSize().x || rx < 0 ||
       ry > _shape.getSize().y || ry < 0 ) {
    return false;   
  }
  else if (_mode != ButtonMode::toggle) {
    _shape.setFillColor(_clickedColor);
    _shape.setTextureRect(_clickedRect);
  }
  return true;
}

void ModulableButton::onMouseRelease(float x, float y)
{
  sf::Vector2f v(getInverseTransform().transformPoint(x,y));
    
  float rx = v.x - _shape.getPosition().x;
  float ry = v.y - _shape.getPosition().y;
  if (rx > _shape.getSize().x || rx < 0 ||
      ry > _shape.getSize().y || ry < 0);
  else clicked();
     
  if (_mode==ButtonMode::toggle)
  {
    if (toggleState())
    {
      _shape.setFillColor(_clickedColor);
      _shape.setTextureRect(_clickedRect);
    }
    else
    {
      _shape.setFillColor(_idleColor);
      _shape.setTextureRect(_idleRect);
    }
  }
  else 
  {  
    _shape.setFillColor(_idleColor);
    _shape.setTextureRect(_idleRect);
  }
}

InstrumentButton::InstrumentButton(InstrumentParameter* p, 
                                   const sf::Texture &texture,
                                   const sf::IntRect &idle,
                                   const sf::IntRect &clicked,
                                   ButtonMode::Mode mode):
ModulableButton(texture,idle,clicked,mode),
_param(p)
{

}

InstrumentButton::~InstrumentButton()
{
}

void InstrumentButton::setParam(InstrumentParameter* p,
                                ButtonMode::Mode mode) 
{
  _param=p;
  _mode=mode;
  if (mode==ButtonMode::toggle && _param)
  {
    if (_param->active()) _shape.setTextureRect(_clickedRect);
    else _shape.setTextureRect(_idleRect);
  }
}

void InstrumentButton::clicked() {
  if (_param)
  {
    switch (_mode)
    {
      case ButtonMode::toggle:
        _param->toggle();
        break;
        
      case ButtonMode::increment:
        (*_param)++;
        break;

      case ButtonMode::decrement:
        (*_param)--;
        break;
        
      case ButtonMode::on:
        _param->on();
        break;
        
      case ButtonMode::off:
        _param->off();
        break;
      default:
        break;
    }
  }
}

Button::Button(const sf::Vector2f& size, const sf::String text,
                ButtonMode::Mode mode) :
ModulableButton(size,text,mode),
_val(NULL)
{
  
}

Button::Button(int* v, 
               const sf::Texture &texture,
               const sf::IntRect &idle,
               const sf::IntRect &clicked,
               ButtonMode::Mode mode) :
ModulableButton(texture,idle,clicked,mode),
_val(v)
{
  
}
           
Button::Button( int* v,  const sf::Vector2f& size, const sf::String text,
                ButtonMode::Mode mode) :
ModulableButton(size,text,mode),
_val(v)
{
  
}
    
Button::~Button()
{

}

void Button::clicked()
{
  if (_val)
  {
    switch (_mode)
    {
      case ButtonMode::toggle:
        if (*_val) {
          *_val=0;
        }
        else {
          *_val=1;
        }
        break;
        
      case ButtonMode::increment:
       ( *_val)++;
        break;

      case ButtonMode::decrement:
        (*_val)--;
        break;
      
      case ButtonMode::interrupt:
      case ButtonMode::on:
        *_val=1;
        break;
        
      case ButtonMode::off:
        *_val=0;
        break;
    }
  
  }
}

void Button::linkTo(int* v,
                    ButtonMode::Mode mode)
{
  _val=v;
  _mode=mode;
  if (mode==ButtonMode::toggle && _val)
  {
    if (*_val) _shape.setTextureRect(_clickedRect);
    else _shape.setTextureRect(_idleRect);
  }
}

ScrollBar::ScrollBar(sf::View& view, unsigned int zone,bool h):
_view(&view),
_bar(),
_decoration(),
_catch(0),
_current_offset(0),
_zone_size(zone),
_horizontal(h)
{
  _bar.setFillColor(sf::Color(142,142,142,200));
  _decoration.setFillColor(sf::Color(100,100,100,200));
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

void ScrollBar::setZoneSize(unsigned int zone)
{
  _zone_size=zone;
  update();
}

bool ScrollBar::onMousePress(float x, float y)
{
  if (_horizontal && _zone_size <= _view->getSize().x) return false;
  else if (!_horizontal && _zone_size <= _view->getSize().y) return false;
  
  if (_bar.getGlobalBounds().contains(x,y))
  {
    if (_horizontal) _catch=x-_bar.getPosition().x;
    else _catch=y-_bar.getPosition().y;
    _bar.setFillColor(sf::Color(142,42,42,255));
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
  (void)x;
  (void)y;
  _bar.setFillColor(sf::Color(142,142,142,200));
}

void ScrollBar::draw (sf::RenderTarget &target, sf::RenderStates states) const
{
  (void)states;
  if ((_horizontal && _zone_size > _view->getSize().x) || 
      (!_horizontal && _zone_size > _view->getSize().y))
  {
    target.draw(_decoration);
    target.draw(_bar);
  }
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
    //La decoration est au bord de la vue     
    _decoration.setPosition(_current_offset,_view->getCenter().y+_view->getSize().y/2.f-12.f);
    //La decoration fait la largeur de la vue
    _decoration.setSize(sf::Vector2f( _view->getSize().x, 12.f));
  }
  else
  {
    _bar.setSize(sf::Vector2f(12,_view->getSize().y*_view->getSize().y/_zone_size));
    _view->setCenter(_view->getCenter().x,_current_offset+_view->getSize().y/2);
    _bar.setPosition(_view->getCenter().x+(_view->getSize().x/2.f)-12,
                      _current_offset + _current_offset*_view->getSize().y/(float)_zone_size);
    //La decoration est au bord de la vue                 
    _decoration.setPosition(_view->getCenter().x+(_view->getSize().x/2.f)-12.f,_current_offset);
    //La decoration fait la largeur de la vue
    _decoration.setSize(sf::Vector2f( 12.f,_view->getSize().y));
  }              
}

SingleProcessButton::SingleProcessButton(const sf::Vector2f& size,
                                         const sf::String text) :
AbstractButton(size,text),
_thread(0)
{
}

SingleProcessButton::SingleProcessButton(const sf::Texture &texture,
                                         const sf::IntRect &idle,
                                         const sf::IntRect &clicked) :
AbstractButton(texture,idle,clicked),
_thread(0)
{
}

SingleProcessButton::~SingleProcessButton()
{
  if (_thread) delete _thread;
}

void SingleProcessButton::clicked()
{
  if (_thread)
  {
    _thread->wait();
    _thread->launch();
  }
}


/*
Fader::Fader(InstrumentParameter* p, const sf::Texture &texture, const sf::IntRect &backRect, const sf::IntRect &faderRect) :
  _param(p),
  _back_sprite(texture, backRect),
  _fader_sprite(texture, faderRect),
  _catch_x(0),
  _catch_y(0),
  _catch_translation(0)
{
  _fader_sprite.setOrigin(faderRect.width/2.f,faderRect.height/2.f);
  _fader_sprite.setPosition(backRect.width/2.f,backRect.height/2.f);
}
*/
Interface::Interface(const sf::Vector2u& zone,const sf::Vector2f &size):
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
  return NULL;
}

void Interface::setViewSize(float x, float y)
{
  _view.setSize(x,y);
  _view.setCenter(x*0.5f,y*0.5f);
  _verticalBar.update();
  _horizontalBar.update();
  
  if (x > _zone.x)
    _view.setCenter(x/2.f,_view.getCenter().y);
  if (y > _zone.y)
    _view.setCenter(_view.getCenter().x,y/2.f);
}

void Interface::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  (void)states;
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

bool Interface::onIt(unsigned int x, unsigned int y, 
                     unsigned int sx, unsigned int sy)
{
  const sf::FloatRect& r= _view.getViewport();
  return !(x > sx*(float)(r.left+r.width) || 
           x < sx*(float)(r.left) || 
           y > sy*(float)(r.top+r.height) ||
           y < sy*(float)(r.top));
}


void Interface::_internalZoneChanged(const sf::Vector2u& nz)
{
  _zone = nz;
  _verticalBar.setZoneSize(nz.y);
  _horizontalBar.setZoneSize(nz.x);
}
