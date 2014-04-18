#include "scope.hpp"

Scope::Scope(const sf::Vector2i& zone) :
Interface(zone, sf::Vector2f(Signal::size,100.f)),
_signal(0),
_pixels(0),
_texture(),
_sprite(),
_y_zoom(1)
{
   addDrawable(&_sprite);
}

Scope::Scope(const sf::Vector2i& zone,Signal* s) :
Interface(zone, sf::Vector2f(Signal::size,100.f)),
_signal(0),
_pixels(0),
_texture(),
_sprite(),
_y_zoom(1)
{
   setSignal(s);
   addDrawable(&_sprite);
}

Scope::~Scope()
{
  if (_pixels) free(_pixels);
}

void Scope::setYZoom(float z)
{
   if (z) _y_zoom=z;
}
void Scope::setSignal(Signal* s)
{
   _signal=s;
   if (_pixels) free(_pixels);
   if (_signal)
   {
      _zone=sf::Vector2i(100.f,Signal::size);
      _texture.create(_zone.x, _zone.y);
      _pixels = (sf::Uint8*) malloc(_zone.x* _zone.y*4);
      _sprite.setTexture(_texture,true);
      _sprite.setOrigin(_zone.x/2,_zone.y/2);
      //_sprite.setPosition(_zone.x/2,_zone.y/2);
      _sprite.setRotation(90);
   }
}
void Scope::update()
{
   if (_pixels)
   {
      for (unsigned int i=0; i < _zone.x*_zone.y;)
      {
         _pixels[i++]=0;
         _pixels[i++]=0;
         _pixels[i++]=0;
         _pixels[i++]=255;
      }
      
      for (unsigned int x=0; x < Signal::size;x++)
      {
         const int y = _signal->samples[x]*_y_zoom*_zone.x*0.5;
         if (y > -_zone.x/2 && y < _zone.x/2)
         {
            _pixels[(x*_zone.x + y + _zone.x/2)*4] = 255;
         }
      }
      _texture.update(_pixels);
   }
}