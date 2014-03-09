
#include "config.hpp"

#include <cstdio>
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include "signal.hpp"
#include "oscillator.hpp"
#include "nelead6.hpp"
#include "settings.hpp"
#include <SFML/Graphics.hpp>
#include <map>
#include "bassdriver.hpp"
#include "interface.hpp"

int main()
{
  srand(time(NULL));
  Settings::getInstance().loadFile("settings.ini");
  unsigned int signalFrequency = GetSettingsFor("Signal/Frequency",44100);
  unsigned int signalRefreshRate = GetSettingsFor("Signal/RefreshRate",50);
  Signal::globalConfiguration(signalFrequency,signalRefreshRate);
  
  
  AudioDriver* driver;
  if (GetSettingsFor("ASIO/UseASIODriver",false))
  {
    driver = new BassAsioDriver();
  }
  else
  {
    driver = new BassDriver();
  }
  
  if (!driver->init(Signal::frequency)) return 0xdead;
  AudioStream stream(Signal::size*2);
  
  sf::RenderWindow window(sf::VideoMode(800, 600), "Millenium Synth");
  
  //Current mouse catcher
  MouseCatcher* currentMouseCatcher=NULL;
  Interface* currentInterfaceCatcher=NULL;
  
  float dt=0.02;
  unsigned int time=0;
  Instrument<NELead6Voice> lead;
  
  
  sf::Texture knobTexture;
  knobTexture.loadFromFile(std::string("img/knob.png"));
  Knob volumeKnob(lead.getParameter(PARAM_INSTRUMENT_VOLUME_ID),
                  knobTexture,
                  sf::IntRect(0,0,360,360),
                  sf::IntRect(360,0,360,360));
                  
  Interface* lolinterface = new Interface(sf::Vector2i(400,400),sf::Vector2f(800,600));    
  lolinterface->addMouseCatcher(&volumeKnob);
  
  Signal output;
  bool sendSignalSuccess=true;
  
  std::map<sf::Keyboard::Key,Note*> notes;
  
  if (!driver->start(&stream)) return 0xdead;
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
            lolinterface->setViewSize(event.size.width,event.size.height);
          }
          break;
        case sf::Event::MouseButtonPressed:
          if (event.mouseButton.button == sf::Mouse::Left)
          {
            if (!currentMouseCatcher)
            {
              sf::Vector2f v = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x,event.mouseButton.y),lolinterface->getView()); 	
              currentMouseCatcher = lolinterface->onMousePress(v.x,v.y);
              currentInterfaceCatcher = lolinterface;
            }
          }
          break;
        case sf::Event::MouseButtonReleased:
          if (event.mouseButton.button == sf::Mouse::Left)
          {
            if (currentMouseCatcher)
            {
              sf::Vector2f v = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x,event.mouseButton.y),currentInterfaceCatcher->getView()); 
              currentMouseCatcher->onMouseRelease(v.x,v.y);
              currentMouseCatcher=NULL;
            }
          }
          break;
        case sf::Event::MouseMoved:
          if (currentMouseCatcher)
          {
            sf::Vector2f v = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x,event.mouseMove.y),currentInterfaceCatcher->getView());
            currentMouseCatcher->onMouseMove(v.x,v.y);
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
    
    
    
    lolinterface->update();

    
    if (sendSignalSuccess)
    {
      //std::cout << "generating output..." << std::endl;
      lead.step(&output);
      sf::Lock lock(stream);
      //std::cout << "try..." << std::endl;
      sendSignalSuccess = stream.writeSignal(output);
    }
    else
    {
      //std::cout << "retry..." << std::endl;
      sf::Lock lock(stream);
      sendSignalSuccess = stream.writeSignal(output);
    }
    
   
    
    window.clear();
    
    window.setView(lolinterface->getView());
    window.draw(*lolinterface);
    
    // Update the window
    window.display();
    
    //std::cout << "CPU usage : " << BASS_ASIO_GetCPU() << std::endl;
  }
  delete driver;
  return 0;
}