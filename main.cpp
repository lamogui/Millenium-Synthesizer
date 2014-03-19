
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
#include "puresquare.hpp"
#include "record.hpp"

int main()
{
  srand(time(NULL));
  Settings::getInstance().loadFile("settings.ini");
  unsigned int signalFrequency = GetSettingsFor("Signal/Frequency",44100);
  unsigned int signalRefreshRate = GetSettingsFor("Signal/RefreshRate",50);
  Signal::globalConfiguration(signalFrequency,signalRefreshRate);
  
  
  AudioDriver* driver;
  #ifdef COMPILE_WINDOWS
  if (GetSettingsFor("ASIO/UseASIODriver",false))
  {
    driver = new BassAsioDriver();
  }
  else
  {
    #endif
    driver = new BassDriver();
    #ifdef COMPILE_WINDOWS
  }
  #endif
  if (!driver->init(Signal::frequency)) return 0xdead;
  AudioStream stream(Signal::size*2);
  
  sf::RenderWindow window(sf::VideoMode(720, 360), "Millenium Synth");
  
  window.setFramerateLimit(2*Signal::refreshRate);
  
  //Current mouse catcher
  MouseCatcher* currentMouseCatcher=NULL;
  Interface* currentInterfaceCatcher=NULL;
  
  float dt=0.02;
  unsigned int mytime=0;
  time_t current;
  time_t retard;
  time (&current);
  time (&retard);
  double diff=0;
  NELead6 lead;      
  Interface* lolinterface = new NELead6Interface(&lead,sf::Vector2f(720,360));    
  
  Signal output;
  bool sendSignalSuccess=true;
  
  std::map<sf::Keyboard::Key,Note*> notes;

  Record r;
  r.init();

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
            if (currentMouseCatcher && currentInterfaceCatcher)
            {
              sf::Vector2f v = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x,event.mouseButton.y),currentInterfaceCatcher->getView()); 
              currentMouseCatcher->onMouseRelease(v.x,v.y);
              currentMouseCatcher=NULL;
            }
          }
          break;
        case sf::Event::MouseMoved:
          if (currentMouseCatcher && currentInterfaceCatcher)
          {
            sf::Vector2f v = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x,event.mouseMove.y),currentInterfaceCatcher->getView());
            currentMouseCatcher->onMouseMove(v.x,v.y);
          }
          break;
          
        case sf::Event::KeyPressed:
          {
            unsigned char id=NOT_A_NOTE;
            switch (event.key.code)
            {
              case sf::Keyboard::Q: id=DO_3; break;
              case sf::Keyboard::Z: id=REb_3; break;
              case sf::Keyboard::S: id=RE_3; break;
              case sf::Keyboard::E: id=MIb_3; break;
              case sf::Keyboard::D: id=MI_3; break;
              case sf::Keyboard::F: id=FA_3; break;
              case sf::Keyboard::T: id=SOLb_3; break;
              case sf::Keyboard::G: id=SOL_3; break;
              case sf::Keyboard::Y: id=LAb_3; break;
              case sf::Keyboard::H: id=LA_4; break;
              case sf::Keyboard::U: id=SIb_4; break;
              case sf::Keyboard::J: id=SI_4; break;
              default: break;
            }
            if (id!=NOT_A_NOTE)
            {
              if (notes.find(event.key.code) == notes.end())
              {
                r.writeNote(id, mytime);
                notes[event.key.code] = new Note(0,id,1.0);
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
      time (&current);
      diff = difftime(current, retard);
      if (diff >1) {
        mytime++;
        retard=current;
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
  r.close();
  return 0;
}
