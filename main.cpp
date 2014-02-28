
#include <cstdio>
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "signal.hpp"
#include "audiostream.hpp"
#include "oscillator.hpp"
#include <SFML/Graphics.hpp>


int main()
{
  srand(time(NULL));
  //sf::View cam(sf::Vector2f(0,0),sf::Vector2f(terrain.width + 1,6.0*terrain.width/8.0));
  sf::RenderWindow window(sf::VideoMode(800, 600), "Millenium Synth");
  
  BASS_Init(-1, Signal::frequency, 0,0,0);
  
  
  float dt=0.02;
  bool use_callback=false;
  
  AudioStream s;
  SinusoidalOscillator uni;
  uni.setFrequency(1.0);
  uni.setAmplitude(1.0);
  
  SinusoidalOscillator osc;
  osc.setFrequency(440.0);
  osc.setAmplitude(0.75);
  
  
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
        default:
          break;
      }
    }
    
    
    if (s.prepareNextBuffer())
    {
      //std::cout << "prepare\n"; 
      Signal* signal = s.getPreparedBuffer();
      
      if (signal)
      {
        //sample* samples = signal->samples;
        //generate unisson
        osc.unisson=&uni.generate();
        //output sinuoidal
        osc.stepfill(signal);
      }
    }
    
    if (!use_callback && s.pushInBass())
    {
      //std::cout << "pushed in bass\n"; 
    }
    
    // Update the window
    window.display();
  }
  return 0xdead;
}