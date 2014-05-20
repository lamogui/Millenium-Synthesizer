#include "scope.hpp"
#include <iostream>

Scope::Scope(const sf::Vector2f& size, bool spectrum) :
Interface(sf::Vector2i(Signal::size,100),size),
_signal(0),
_pixels(0),
_color(255,255,255,255),
_texture(),
_back(sf::Vector2f(Signal::size,100)),
_sprite(),
_y_zoom(1),
_spectrum(spectrum)
{
  _back.setFillColor(sf::Color(42,42,42,128));
  addDrawable(&_sprite);
  addDrawable(&_back);
}

Scope::Scope(const sf::Vector2f& size,Signal* s, bool spectrum) :
Interface(sf::Vector2i(Signal::size,100),size),
_signal(0),
_pixels(0),
_color(255,255,255,255),
_texture(),
_back(sf::Vector2f(Signal::size,100)),
_sprite(),
_y_zoom(1),
_spectrum(spectrum)
{
  _back.setFillColor(sf::Color(42,42,42,128));
  setSignal(s);
  addDrawable(&_sprite);
  addDrawable(&_back);
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
      setColor(_color);
      
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
      const unsigned int s = (_texture.getSize().x*_texture.getSize().y) << 2;
      for (unsigned int i=3; i < s;i+=4)
      {
         _pixels[i]=0;
      }

      const unsigned int l = Signal::size < _texture.getSize().y ? Signal::size : _texture.getSize().y;
      //std::cout << "l" << l  << "signal size " << Signal::size << "y" << _texture.getSize().y << "x"  << _texture.getSize().x << std::endl;

      if (!_spectrum) {

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
      }
      else {
        for (unsigned int x=0; x < l;x++)
        {
          int fakey = _signal->samples[x]*_texture.getSize().x*_y_zoom ;
          fakey += _texture.getSize().x >> 1;
          fakey >>= 1;
          fakey = fakey > (int)_texture.getSize().x ? _texture.getSize().x : fakey;
          const int y = fakey < 0 ? 0 : fakey;

          for (int i=0; i<y; i++) {
            _pixels[(x*_texture.getSize().x + (_texture.getSize().x-i))*4+3] = 255;
          }
        }
      }
      _texture.update(_pixels);
   }
   for (unsigned int i=0; i < _mouseCatcher.size();i++)
   {
     _mouseCatcher[i]->update(); 
   }
}

void Scope::setColor(const sf::Color& color)
{
  _color = color;
  if (_pixels)
  {
    unsigned p = _texture.getSize().x* _texture.getSize().y*4;
    for (unsigned int i=0; i < p;)
    {
      _pixels[i++]=_color.r;
      _pixels[i++]=_color.g;
      _pixels[i++]=_color.b;
      _pixels[i++]=0;
    }
  }
}

void Scope::setSpectrum(bool spectrum) {
  _spectrum=spectrum;
}

