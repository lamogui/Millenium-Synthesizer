
#include <cstdio>
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "signal.hpp"
#include <SFML/Graphics.hpp>


int main()
{
  srand(time(NULL));
  //sf::View cam(sf::Vector2f(0,0),sf::Vector2f(terrain.width + 1,6.0*terrain.width/8.0));
  sf::RenderWindow window(sf::VideoMode(800, 600), "Millenium Synth");
  
  window.setFramerateLimit(50);
  float dt=0.02;

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
          {
            sf::View newView(sf::FloatRect(0,0,event.size.width,event.size.height));
            window.setView(newView);
          }
          break;
        case sf::Event::MouseButtonPressed:
          if (event.mouseButton.button == sf::Mouse::Left)
          {
          }
          break;
        default:
          break;
      }
    }
    // Update the window
    window.display();
  }
  return 0xdead;
}