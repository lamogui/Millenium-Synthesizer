
#include <cstdio>
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "signal.hpp"
#include "audiostream.hpp"
#include "oscillator.hpp"
#include "nelead6.hpp"
#include <SFML/Graphics.hpp>
#include <map>

int main()
{
  srand(time(NULL));
  //sf::View cam(sf::Vector2f(0,0),sf::Vector2f(terrain.width + 1,6.0*terrain.width/8.0));
  sf::RenderWindow window(sf::VideoMode(800, 600), "Millenium Synth");
  
  BASS_Init(-1, Signal::frequency, 0,0,0);
  
  
  float dt=0.02;
  bool use_callback=false;
  unsigned int time=0;
  
  AudioStream s;
  NELead6 lead;
  
  std::map<sf::Keyboard::Key,Note*> notes;
  
  HSTREAM stream = s.createCompatibleBassStream(use_callback);
  std::cout << "bass start " << BASS_Start() << std::endl;
  std::cout << "bass channel start " << BASS_ChannelPlay(stream,TRUE) << std::endl;
  
  if (use_callback) 
  {
    window.setFramerateLimit(50); //Detruit tout ne pas activer
    // update period min 5 ms
    if (BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD,10))
    {
      std::cout << "Bass period correctly set to " << BASS_GetConfig(BASS_CONFIG_UPDATEPERIOD) << " ms" << std::endl;
    }
    else
    {
      std::cerr << "Bass period error unable set it to " << BASS_GetConfig(BASS_CONFIG_UPDATEPERIOD) << " ms" << std::endl;
    }
    
    if (BASS_SetConfig(BASS_CONFIG_BUFFER,AudioStream::nBuffers*1000/Signal::refreshRate))
    {
      std::cout << "Bass buffer correctly set to " << AudioStream::nBuffers*1000/Signal::refreshRate << " ms" << std::endl;
    }
    else
    {
      std::cerr << "Bass buffer error unable set it to " << AudioStream::nBuffers*1000/Signal::refreshRate << " ms" << std::endl;
    }
  }

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
          
        case sf::Event::KeyPressed:
          {
            float f=0;
            switch (event.key.code)
            {
              case sf::Keyboard::Q: f=261.63; break;
              case sf::Keyboard::Z: f=277.18; break;
              case sf::Keyboard::S: f=293.66; break;
              case sf::Keyboard::E: f=311.13; break;
              case sf::Keyboard::D: f=329.63; break;
              case sf::Keyboard::F: f=349.23; break;
              case sf::Keyboard::T: f=369.99; break;
              case sf::Keyboard::G: f=392.00; break;
              case sf::Keyboard::Y: f=415.30; break;
              case sf::Keyboard::H: f=440.00; break;
              case sf::Keyboard::U: f=466.16; break;
              case sf::Keyboard::J: f=493.88; break;
              default: break;
            }
            if (f)
            {
              if (notes.find(event.key.code) == notes.end())
              {
                notes[event.key.code] = new Note(time,f,1.0);
                lead.playNote(*notes[event.key.code]);
              }  
            }
          }
          break;
        case sf::Event::KeyReleased:
          if (notes.find(event.key.code) != notes.end())
          {
            delete notes[event.key.code];
            notes.erase(event.key.code);
          }
          break;
        default:
          break;
      }
    }
    
    
    if (s.prepareNextBuffer())
    {
      Signal* signal = s.getPreparedBuffer();
      if (signal)
      {
        lead.step(signal);
        time++;
      }
      
    }
    sf::sleep(sf::microseconds(1000));
    
    if (!use_callback && s.pushInBass())
    {
    }
    
    
    
    // Update the window
    window.display();
  }
  return 0xdead;
}