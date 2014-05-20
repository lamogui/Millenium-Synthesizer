#ifndef SCOPE__
#define SCOPE__
#include "signal.hpp"
#include "interface.hpp"
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
      
      
      void setColor(const sf::Color& color);
      
      void setFadeColor(const sf::Color& colorInit, const sf::Color& colorEnd, bool axe);

   protected:
      Signal* _signal;
      sf::Uint8* _pixels;
      sf::Color _color;
      sf::Texture _texture;
      sf::RectangleShape _back;
      sf::Sprite _sprite;
      float _y_zoom;
      bool _spectrum;
};


#endif
