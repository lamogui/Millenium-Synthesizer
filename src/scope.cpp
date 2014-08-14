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
  Interface(sf::Vector2u(Signal::size,100),size),
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
  _spectrum(spectrum),
  _autoscaley(0),
  _modeButton(sf::Vector2f(80.f,18.f), "FFT" ),
  _autoYScaleButton(sf::Vector2f(80.f,18.f), "Auto Scale" )
{
  _back.setFillColor(sf::Color(42,42,42,128));
  _modeButton.setCallback(this,&Scope::setSpectrum, &Scope::isSpectrum);
  _autoYScaleButton.linkTo(&_autoscaley);
  addMouseCatcher(&_modeButton);
  addMouseCatcher(&_autoYScaleButton);
  addDrawable(&_sprite);
  addDrawable(&_back);
  _allocate();
}

Scope::Scope(const sf::Vector2f& size,Signal* s, bool spectrum) :
  Interface(sf::Vector2u(Signal::size,100),size),
  _signal(s),
  _fft(0),
  _pixels(0),
  _color(255,255,255,255),
  _texture(),
  _back(sf::Vector2f(Signal::size,100)),
  _sprite(),
  _y_zoom(1),
  _update_time(0),
  _time(0),
  _spectrum(spectrum),
  _autoscaley(0),
  _modeButton(sf::Vector2f(80.f,18.f), "FFT" ),
  _autoYScaleButton(sf::Vector2f(80.f,18.f), "Auto Scale" )
{
  _back.setFillColor(sf::Color(42,42,42,128));
  _modeButton.setCallback(this,&Scope::setSpectrum, &Scope::isSpectrum);
  _autoYScaleButton.linkTo(&_autoscaley);
  addMouseCatcher(&_modeButton);
  addMouseCatcher(&_autoYScaleButton);
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
    if (_spectrum) { 
      const unsigned samples = GetSettingsFor("FFT/Samples",4096);
      const unsigned size=samples < 4096 ? samples: 4096;
      _fft=new FFT(samples); 
      _internalZoneChanged(sf::Vector2u(size,_zone.y));
    }
    else {
      _internalZoneChanged(sf::Vector2u(Signal::size,_zone.y));
    } 
    _texture.create(_zone.y, _zone.x);
    _back.setSize(sf::Vector2f(_zone.x,_zone.y));
    unsigned p = _texture.getSize().x* _texture.getSize().y*4;
    _pixels = (sf::Uint8*) malloc(p);
    setColor(_color);

    _sprite.setTexture(_texture,true);
    _sprite.setOrigin(_zone.y/2,_zone.x/2);
    if (_spectrum)  _sprite.setPosition(_zone.x/2,_zone.y/2-12.f);
    else _sprite.setPosition(_zone.x/2,_zone.y/2);
    _sprite.setRotation(90);
  }
  
}

void Scope::update()
{
  Interface::update();
  sf::Vector2i position=getPosition();
  _modeButton.setPosition(position.x+10,position.y+10);
  _autoYScaleButton.setPosition(position.x+10,position.y+30);
  
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
      float smax = 0.0;
      for (unsigned int x=0; x < l;x++) {
        const float a=fabs(_signal->samples[x]);
        if (a > smax) smax=a;
      }
      const float _y_factor = _autoscaley && smax > 0.001 ?  
                  (_texture.getSize().x-20) *_y_zoom * 0.5 * 1/smax :
                  _texture.getSize().x * 0.5 * _y_zoom;
      for (unsigned int x=0; x < l;x++)
      {
        const int y = _signal->samples[x]*_y_factor;

        if (y-1 >= -(int)(_texture.getSize().x>>1) && y+1 < (int)(_texture.getSize().x>>1))
        {
		  const unsigned int offset = ((x*_texture.getSize().x + y + (_texture.getSize().x>>1))<<2) + 3;
          _pixels[offset] = 255;
          _pixels[offset+4] = 120;
          _pixels[offset-4] = 120;
		  /*
          _pixels[(x*_texture.getSize().x + y + _texture.getSize().x/2)*4 + 3] = 255;
          _pixels[(x*_texture.getSize().x + y + 1 + _texture.getSize().x/2)*4 + 3] = 120;
          _pixels[(x*_texture.getSize().x + y - 1 + _texture.getSize().x/2)*4 + 3] = 120;*/

        }
      }
    }
    else if (_fft) {
      _fft->pushSignal(*_signal);
      _fft->computeModule();
      const unsigned int l = _fft->size() < _texture.getSize().y ? _fft->size() : _texture.getSize().y;
      const unsigned int sy =_texture.getSize().x;
      float smax = 0.0;
      for (unsigned int x=0; x < l;x++) {
        const float a=2.0*_fft->getModule()[x];
        //if (a < 0) std::cerr << "Erreur dans la fft !! " << a << std::endl;
        if (sqrt(a) > smax) smax=sqrt(a);
      }
      //std::cout << " smax " << smax << std::endl;
      const float _y_factor = _autoscaley && smax > 0.001 ?  
                  (_texture.getSize().x-20) *_y_zoom * 1/smax :
                  _texture.getSize().x * _y_zoom;
      for (unsigned int x=0; x < l;x++)
      {
        const unsigned int fakey = sqrt(2.0*_fft->getModule()[x])*_y_factor ;
        const unsigned int y = fakey > sy ? sy : fakey;
        const unsigned delta_x = x*sy*4 + 3;
        
        for (unsigned int i=0; i<y; i++) {
          _pixels[delta_x + ((sy-i-1)<<2)] = 255;
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
          //unsigned p = _texture.getSize().x* _texture.getSize().y*4;
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
