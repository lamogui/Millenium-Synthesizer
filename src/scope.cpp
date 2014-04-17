#include "scope.hpp"

Scope::Scope(const sf::Vector2i& zone) :
Interface(zone, sf::Vector2f(Signal::size,100.f)),
_signal(0),
_pixels(0),
_texture(),
_sprite(),
_y_zoom(1)
{
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
}

Scope::~Scope()
{
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
      _zone=sf::Vector2i(Signal::size,100.f);
      _texture.create(_zone.x, _zone.y);
      _pixels = (sf::Uint8*) malloc(_zone.x* _zone.y*4);
      _sprite.setTexture(_texture,true);
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
         const int y = _signal->samples[x]*_y_zoom*_zone.y*0.5;
         if (y > -50 && y < 50)
            _pixels[x*y*4] = 255;
      }
      _texture.update(_pixels);
   }
}