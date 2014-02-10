
#ifndef __TERRAIN
#define __TERRAIN

#include <SFML/Graphics.hpp>

class Terrain : public sf::Drawable
{
  public:
    Terrain() :
    width(68),
    height(105)
    {
    
    }
    virtual ~Terrain()
    {
    }
    
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
    float width;
    float height;
    
    virtual void generate() = 0;
    
    
};

class BasicTerrain : public Terrain
{
  public:
    BasicTerrain();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    virtual void generate();
    
  private:
    sf::Color _grassColor;
    sf::Color _linesColor;
    sf::RectangleShape _grass;
    sf::CircleShape _middleCircle;
    sf::RectangleShape _middleLine;
    
    sf::RectangleShape _upSurface;
    sf::RectangleShape _downSurface;
    
    sf::RectangleShape _upGoalSurface;
    sf::RectangleShape _downGoalSurface;
    
    sf::CircleShape _upCircle;
    sf::CircleShape _downCircle;
};



#endif