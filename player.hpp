#ifndef __PLAYER
#define __PLAYER

#include <SFML/Graphics.hpp>

class Player : public sf::Drawable, public sf::Transformable
{
  public:
    Player();
    virtual ~Player();
    
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    virtual void update(float dt);
    
  protected:
    unsigned int state;
    sf::CircleShape _head;
};

#endif