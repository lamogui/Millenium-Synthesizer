/****************************************************************************
Nom ......... : scope.hpp
Role ........ : Définie une interface de visualisation des signaux 
                configurable en oscilloscope ou analyseur de spectre
Auteur ...... : Julien DE LOOR & Kwon-Young CHOI
Version ..... : V1.7 olol
Licence ..... : © Copydown™
****************************************************************************/


#ifndef SCOPE__
#define SCOPE__
#include "signal.hpp"
#include "interface.hpp"
#include "fft.hpp"
#include <cstdlib>

class Scope : public Interface
{
   public:
      Scope(const sf::Vector2f& size, bool spectrum=false);
      Scope(const sf::Vector2f& size,Signal* s, bool spectrum=false);
      virtual ~Scope();
      
      void setYZoom(float z);
      void setSignal(Signal* s);

      inline bool isSpectrum() {
        return _spectrum;
      }

      void setSpectrum(bool spectrum);

      virtual void update();
      
      inline void setUpdateRate(unsigned int t) { _update_time=t;}
      
      void setColor(const sf::Color& color);
      
      void setFadeColor(const sf::Color& colorInit, const sf::Color& colorEnd, bool axe);

   protected:
      void _allocate();
   
      Signal* _signal;
      FFT* _fft;
      sf::Uint8* _pixels;
      sf::Color _color;
      sf::Texture _texture;
      sf::RectangleShape _back;
      sf::Sprite _sprite;
      float _y_zoom;
      unsigned int _update_time;
      unsigned int _time;
      bool _spectrum;
      int _autoscaley;
      ModulableCallbackButton<Scope> _modeButton;
      Button _autoYScaleButton;
};

#endif
