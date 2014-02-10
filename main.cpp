
#include <cstdio>
#include "terrain.hpp"

int main()
{
  BasicTerrain terrain;
  sf::View cam(sf::Vector2f(0,0),sf::Vector2f(terrain.width + 1,6.0*terrain.width/8.0));
  sf::RenderWindow window(sf::VideoMode(800, 600), "Jeu Foot");
  
  window.setView(cam);
  window.setFramerateLimit(60);
  while (window.isOpen())
  {
  // Process events
    sf::Event event;
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
    
    
    // Clear screen
    window.clear();
   
   window.draw(terrain);
   
    // Update the window
    window.display();
  }
  return 0xdead;
}