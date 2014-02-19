#ifndef __PLAYER
#define __PLAYER

#include <SFML/Graphics.hpp>
#include <vector>
#include "soccerball.hpp"

class Player : public sf::Drawable, public sf::Transformable
{
  public:
    Player();
    virtual ~Player();
    
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    virtual void think(const SoccerBall& ball, std::vector<Player*>& up_team, std::vector<Player*>& down_team);
    
    sf::Vector2f _acceleration;
    
    virtual void update(float dt);
    
    
  protected:
    unsigned int _state;
    sf::CircleShape _head;
    sf::CircleShape _eye1;
    sf::CircleShape _eye2;
    sf::RectangleShape _corpse;
    sf::Vector2f _velocity;
    
};

#endif