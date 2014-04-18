#include "scope.hpp"
#include <iostream>

Scope::Scope(const sf::Vector2f& size) :
Interface(sf::Vector2i(Signal::size,100),size),
_signal(0),
_pixels(0),
_texture(),
_sprite(),
_y_zoom(1)
{
   addDrawable(&_sprite);
}

Scope::Scope(const sf::Vector2f& size,Signal* s) :
Interface(sf::Vector2i(Signal::size,100),size),
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
      _texture.create(_zone.y, _zone.x);
      unsigned p = _texture.getSize().x* _texture.getSize().y*4;
      _pixels = (sf::Uint8*) malloc(p);
      for (unsigned int i=0; i < p;)
      {
         _pixels[i++]=255;
         _pixels[i++]=0;
         _pixels[i++]=0;
         _pixels[i++]=255;
      }
      
      _sprite.setTexture(_texture,true);
      _sprite.setOrigin(_zone.y/2,_zone.x/2);
      _sprite.setPosition(_zone.x/2,_zone.y/2);
      _sprite.setRotation(90);
   }
}
void Scope::update()
{
   if (_pixels)
   {
      const int s = _texture.getSize().x*_texture.getSize().y*4;
      for (unsigned int i=3; i < s;i+=4)
      {
         _pixels[i]=0;
      }
      
      const int l = Signal::size < _texture.getSize().y ? Signal::size : _texture.getSize().y;
      //std::cout << "l" << l  << "signal size " << Signal::size << "y" << _texture.getSize().y << "x"  << _texture.getSize().x << std::endl;
      for (unsigned int x=0; x < l;x++)
      {
         const int y = _signal->samples[x]*_y_zoom*_texture.getSize().x*0.5;
         
         if (y-1 >= -(int)(_texture.getSize().x>>1) && y+1 < (int)(_texture.getSize().x>>1))
         {
            _pixels[(x*_texture.getSize().x + y + _texture.getSize().x/2)*4 + 3] = 255;
            _pixels[(x*_texture.getSize().x + y + 1 + _texture.getSize().x/2)*4 + 3] = 120;
            _pixels[(x*_texture.getSize().x + y - 1 + _texture.getSize().x/2)*4 + 3] = 120;
         }
        /* else {
          std::cout << "y" << y << std::endl;
         }*/

      }
      _texture.update(_pixels);
   }
   for (unsigned int i=0; i < _mouseCatcher.size();i++)
  {
    _mouseCatcher[i]->update(); 
  }
}