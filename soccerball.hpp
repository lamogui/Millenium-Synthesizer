
 #ifndef __SOCCERBALL
 #define __SOCCERBALL
 
 #include <SFML/Graphics.hpp>
 
class SoccerBall : public sf::CircleShape
{
  public:
    SoccerBall();
    virtual ~SoccerBall();
    virtual void update(float dt);
    bool intersect(float x, float y);
    
    float z;
    
    sf::Vector3f velocity;
    sf::Vector3f acceleration;
    
  protected:
    sf::Texture _tex;
    int _frame;
    float _timer;
    
  
};
 
 #endif