/****************************************************************************
Nom ......... : scope.hpp
Role ........ : Implémente une interface de visualisation des signaux 
                configurable en oscilloscope ou analyseur de spectre
Auteur ...... : Julien DE LOOR & Kwon-Young CHOI
Version ..... : V1.7 olol
Licence ..... : © Copydown™
****************************************************************************/

#include "scope.hpp"
#include <iostream>
#include "settings.hpp"


#ifdef _MSC_VER
#pragma warning( disable : 4244 )
#endif

Scope::Scope(const sf::Vector2f& size, bool spectrum) :
  Interface(sf::Vector2i(Signal::size,100),size),
  _signal(0),
  _fft(0),
  _pixels(0),
  _color(255,255,255,255),
  _texture(),
  _back(sf::Vector2f(Signal::size,100)),
  _sprite(),
  _y_zoom(1),
  _update_time(0),
  _time(0),
  _spectrum(spectrum)
{
  _back.setFillColor(sf::Color(42,42,42,128));
  addDrawable(&_sprite);
  addDrawable(&_back);
  _allocate();
}

Scope::Scope(const sf::Vector2f& size,Signal* s, bool spectrum) :
  Interface(sf::Vector2i(Signal::size,100),size),
  _signal(0),
  _fft(0),
  _pixels(0),
  _color(255,255,255,255),
  _texture(),
  _back(sf::Vector2f(Signal::size,100)),
  _sprite(),
  _y_zoom(1),
  _update_time(0),
  _time(0),
  _spectrum(spectrum)
{
  _back.setFillColor(sf::Color(42,42,42,128));
  addDrawable(&_sprite);
  addDrawable(&_back);
  _allocate();
}

Scope::~Scope()
{
  if (_pixels) free(_pixels);
  if (_fft) delete(_fft);
}

void Scope::setYZoom(float z)
{
  if (z) _y_zoom=z;
}
void Scope::setSignal(Signal* s)
{
  _signal=s;
  _allocate();
}

void Scope::_allocate()
{
  if (_pixels) free(_pixels);
  if (_fft) delete _fft;
  _fft=0;
  _pixels=0;
  if (_signal)
  {
    if (_spectrum) _fft=new FFT(GetSettingsFor("FFT/Samples",16384));
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
  Interface::update();

  if (_pixels && _time++ > _update_time && _signal)
  {
    _time=0;
    const unsigned int s = (_texture.getSize().x*_texture.getSize().y) << 2;
    for (unsigned int i=3; i < s;i+=4)
    {
      _pixels[i]=0;
    }

    if (!_spectrum) {
      const unsigned int l = Signal::size < _texture.getSize().y ? Signal::size : _texture.getSize().y;
      for (unsigned int x=0; x < l;x++)
      {
        const int y = _signal->samples[x]*_y_zoom*_texture.getSize().x*0.5;

        if (y-1 >= -(int)(_texture.getSize().x>>1) && y+1 < (int)(_texture.getSize().x>>1))
        {

          _pixels[(x*_texture.getSize().x + y + _texture.getSize().x/2)*4 + 3] = 255;
          _pixels[(x*_texture.getSize().x + y + 1 + _texture.getSize().x/2)*4 + 3] = 120;
          _pixels[(x*_texture.getSize().x + y - 1 + _texture.getSize().x/2)*4 + 3] = 120;

        }
      }
    }
    else if (_fft) {
      _fft->pushSignal(*_signal);
      _fft->computeModule();
      const unsigned int l = _fft->size() < _texture.getSize().y ? _fft->size() : _texture.getSize().y;
      const unsigned int sy =_texture.getSize().x;
      for (unsigned int x=0; x < l;x++)
      {
        int fakey = sqrt(_fft->getModule()[x])*sy*_y_zoom ;
        fakey = fakey > (int)sy ? sy : fakey;
        const int y = fakey < 0 ? 0 : fakey;
        const unsigned delta_x = x*sy*4;
        
        for (int i=0; i<y; i++) {
          _pixels[delta_x + (sy-i-1)*4 + 3] = 255;
        }
      }
    }
    else {
      return; //no need update
    }
    _texture.update(_pixels);
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
  _allocate();
}

void Scope::setFadeColor(const sf::Color& colorInit, const sf::Color& colorEnd, bool axe)
{
  if (axe) {
    _color = colorInit;
    if (_pixels)
    {
      unsigned int sizeX=_texture.getSize().y;
      unsigned int sizeY=_texture.getSize().x;
      for (unsigned int i=0; i<sizeX ; i++) {
        float k=(float)i/(float)sizeX;
        for (unsigned int j=0; j<sizeY ; j++) {
          unsigned p = _texture.getSize().x* _texture.getSize().y*4;
          unsigned int a=(i*sizeY+j)*4;
          _pixels[a]=_color.r*k+colorEnd.r*(1-k);
          _pixels[a+1]=_color.g*k+colorEnd.g*(1-k);
          _pixels[a+2]=_color.b*k+colorEnd.b*(1-k);
          _pixels[a+3]=0;
        }
      }
    }
  }
}


