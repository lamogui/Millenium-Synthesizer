#ifndef SCOPE__
#define SCOPE__
#include "signal.hpp"
#include "interface.hpp"
#include <cstdlib>

class Scope : public Interface
{
   public:
      Scope(const sf::Vector2f& size);
      Scope(const sf::Vector2f& size,Signal* s);
      virtual ~Scope();
      
      void setYZoom(float z);
      void setSignal(Signal* s);
      void update();
      
      
   protected:
      Signal* _signal;
      sf::Uint8* _pixels;
      sf::Texture _texture;
      sf::Sprite _sprite;
      float _y_zoom;
};


#endif