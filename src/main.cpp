
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
#include "careme.hpp"

int main(int argc, char** argv)
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
  
  window.setFramerateLimit(Signal::refreshRate<<1);
  
  //Current mouse catcher
  MouseCatcher* currentMouseCatcher=NULL;
  Interface* currentInterfaceCatcher=NULL;
  
  float dt=0.02;
  unsigned int time=0;
  
  
  AbstractInstrument* myInstrument=NULL;      
  Interface* myInterface=NULL;
  
  if (argc == 2)
  {
    if (std::string("careme") == argv[1]) 
    {
      myInstrument = new Careme;
      myInterface = new CaremeInterface((Careme*) myInstrument,sf::Vector2f(720,360));
    }
    else if (std::string("puresquare") == argv[1]) 
    {
      myInstrument = new PureSquare;
      myInterface = new PureSquareInterface((PureSquare*) myInstrument,sf::Vector2f(720,360));
    }
    else 
    {
      myInstrument = new NELead6;
      myInterface = new NELead6Interface((NELead6*) myInstrument,sf::Vector2f(720,360));
      
    }
    
  }
  else 
  {
    myInstrument = new NELead6;
    myInterface = new NELead6Interface((NELead6*) myInstrument,sf::Vector2f(720,360));
  }

  Signal leftout, rightout;
  bool sendSignalSuccess=true;
  
  std::map<sf::Keyboard::Key,Note*> notes;

  Record r;
  //r.init();

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
            myInterface->setViewSize(event.size.width,event.size.height);
          }
          break;
        case sf::Event::MouseButtonPressed:
          if (event.mouseButton.button == sf::Mouse::Left)
          {
            if (!currentMouseCatcher)
            {
              sf::Vector2f v = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x,event.mouseButton.y),myInterface->getView()); 	
              currentMouseCatcher = myInterface->onMousePress(v.x,v.y);
              currentInterfaceCatcher = myInterface;
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
              case sf::Keyboard::K: id=DO_4; break;
              case sf::Keyboard::O: id=REb_4; break;
              case sf::Keyboard::L: id=RE_4; break;
              case sf::Keyboard::P: id=MIb_4; break;
              case sf::Keyboard::M: id=MI_4; break;
              default: break;
            }
            if (id!=NOT_A_NOTE)
            {
              if (notes.find(event.key.code) == notes.end())
              {
                //r.writeNote(id, time);
                //notes[event.key.code] = reccord.createNote(time,id,1.0);
                //vector.push_back(Note(time,id,1.0))
                //return & (vector[vector.size-1]);
                notes[event.key.code] = new Note(time,id,1.0);
                myInstrument->playNote(*notes[event.key.code]);
              }  
            }
          }
          break;
        case sf::Event::KeyReleased:
          if (notes.find(event.key.code) != notes.end())
          {
            notes[event.key.code]->lenght= time - notes[event.key.code]->start;
            delete notes[event.key.code]; //a virer
            notes.erase(event.key.code);
          }
          break;
        default:
          break;
      }
      }
    
    
    
    myInterface->update();

    
    if (sendSignalSuccess)
    {
      //std::cout << "generating output..." << std::endl;
      time++;
      myInstrument->step(&leftout, &rightout);
      sf::Lock lock(stream);
      //std::cout << "try..." << std::endl;
      sendSignalSuccess = stream.writeStereoSignal(leftout, rightout);
    }
    else
    {
      //std::cout << "retry..." << std::endl;
      sf::Lock lock(stream);
      sendSignalSuccess = stream.writeStereoSignal(leftout, rightout);
    }
    
   
    
    window.clear();
    
    window.setView(myInterface->getView());
    window.draw(*myInterface);
    
    // Update the window
    window.display();
    
    //std::cout << "CPU usage : " << BASS_ASIO_GetCPU() << std::endl;
  }
  
  //
  delete driver;
  //r.close();
  return 0;
}
