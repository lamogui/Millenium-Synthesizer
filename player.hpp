#ifndef __PLAYER
#define __PLAYER

#include <SFML/Graphics.hpp>

class Player : public sf::Drawable
{
  public:
    Player();
    virtual ~Player();
    
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    
  protected:
    
};

#endif