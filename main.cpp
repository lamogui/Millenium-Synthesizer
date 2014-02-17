
#include <cstdio>
#include "terrain.hpp"
#include "soccerball.hpp"
int main()
{
  srand(time(NULL));
  BasicTerrain terrain;
  SoccerBall ball;
  
  sf::View cam(sf::Vector2f(0,0),sf::Vector2f(terrain.width + 1,6.0*terrain.width/8.0));
  sf::RenderWindow window(sf::VideoMode(800, 600), "Jeu Foot");
  
  window.setView(cam);
  window.setFramerateLimit(60);
  
  
  
  float dt = 0.16;
  
  while (window.isOpen())
  {
  // Process events
    sf::Event event;
    
    ball.acceleration = sf::Vector3f(0,0,0);
    while (window.pollEvent(event))
    {
      
      switch (event.type)
      {
        case sf::Event::Closed:
          window.close(); // Close window : exit
          break;
        case sf::Event::Resized:
          cam.setSize(terrain.width + 1,terrain.width*event.size.height/event.size.width);
          window.setView(cam);
          break;
        case sf::Event::MouseButtonPressed:
          if (event.mouseButton.button == sf::Mouse::Left)
          {
            ball.acceleration += sf::Vector3f((50 - rand()%50) * 0.1, (50 - rand()%50) * 0.1, 80);
          }
          break;
        default:
          break;
      }
    }
    
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && cam.getCenter().y < terrain.height/2) {
      cam.move(0.0,0.4);
      window.setView(cam);
    }
    
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && cam.getCenter().y > -terrain.height/2) {
      cam.move(0.0,-0.4);
      window.setView(cam);
    }
    
    terrain.update();
    
    //frottements de l'air
    ball.acceleration -= ball.velocity*0.04f;
    //poids
    ball.acceleration.z -= 1.2f*dt;
    
    ball.update(dt);
    
    if (ball.z < 0.0) { ball.z=0.0;ball.acceleration.z = abs(ball.acceleration.z)*0.8; }
    if (ball.getPosition().x < -terrain.width/2) ball.setPosition(-terrain.width/2,ball.getPosition().y);
    if (ball.getPosition().x > terrain.width/2) ball.setPosition(terrain.width/2,ball.getPosition().y);
    if (ball.getPosition().y < -terrain.height/2) ball.setPosition(ball.getPosition().x,-terrain.height/2);
    if (ball.getPosition().y > terrain.height/2) ball.setPosition(ball.getPosition().x,terrain.height/2);
    
    // Clear screen
    window.clear();
   
    window.draw(terrain);
    window.draw(ball);
    // Update the window
    window.display();
  }
  return 0xdead;
}