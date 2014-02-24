
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
  
  BASS_Init(-1, Signal::frequency, 0,0,0);
  
  //window.setFramerateLimit(50); //Detruit tout ne pas activer
  float dt=0.02;
  
  Signal s;
  HSTREAM stream = s.createCompatibleBassStream(false);
  std::cout << "bass start " << BASS_Start() << std::endl;
  std::cout << "bass channel start " << BASS_ChannelPlay(stream,TRUE) << std::endl;
  
  int delta=0;
  
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
    
    
    if (s.prepareNextBuffer())
    {
      //std::cout << "prepare\n"; 
      sample* samples = s.getPreparedBuffer();
      for (int i=0;i < Signal::size-1;i+=2)
      {
        samples[i] = 0.3*sin(2.0*3.1415*441.0*(delta + i/2)/(float)Signal::frequency);
        samples[i+1] = samples[i];
      }
      delta+=Signal::size/2;
    }
    
    if (s.pushInBass())
    {
      //std::cout << "pushed in bass\n"; 
    }
    
    // Update the window
    window.display();
  }
  return 0xdead;
}