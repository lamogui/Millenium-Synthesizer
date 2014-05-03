
#ifdef _MSC_VER
#    pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

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
#include "scope.hpp"

sf::Font globalfont; 

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
  
  unsigned int streamSize=Signal::size<<2;
  #ifdef COMPILE_WINDOWS
  if (GetSettingsFor("ASIO/UseASIODriver",false)) streamSize=Signal::size<<2;
  #endif
  AudioStream stream(streamSize);
  
  AbstractInstrument* myInstrument=NULL;      
  Interface* myInterface=NULL;
  
  //Window Borders
  float clientWinSize_x=Signal::size/*+205-40*/;
  float clientWinSize_y=360+100;
  float borderWinSize_up=40;
  float borderWinSize_down=20;
  float borderWinSize_right=20;
  float borderWinSize_left=20;
  bool onMoveWin=false;
  bool onResizeWin=false;
  int onClose=0;
  
  if (argc == 2)
  {
    if (std::string("careme") == argv[1]) 
    {
      myInstrument = new Careme;
      myInterface = new CaremeInterface((Careme*) myInstrument,sf::Vector2f(clientWinSize_x,360));
    }
    else if (std::string("puresquare") == argv[1]) 
    {
      clientWinSize_x=720;
      myInstrument = new PureSquare;
      myInterface = new PureSquareInterface((PureSquare*) myInstrument,sf::Vector2f(clientWinSize_x,360));
    }
    else 
    {
      myInstrument = new NELead6;
      myInterface = new NELead6Interface((NELead6*) myInstrument,sf::Vector2f(clientWinSize_x,360));
      
    }
  }
  else 
  {
    myInstrument = new NELead6;
    myInterface = new NELead6Interface((NELead6*) myInstrument,sf::Vector2f(clientWinSize_x,360));
  }
  
  
  globalfont.loadFromFile("fonts/unispace rg.ttf");
  
  
  sf::Vector2i previousWinPos;
  sf::Vector2i previousMousePos;
  sf::Vector2u previousWinSize;
  Button closeButton(sf::Vector2f(borderWinSize_right+borderWinSize_left,borderWinSize_up*0.5f),"X");
  
  sf::ConvexShape resizeTriangle;
  resizeTriangle.setPointCount(3);
  resizeTriangle.setPoint(0, sf::Vector2f(15, 0));
  resizeTriangle.setPoint(1, sf::Vector2f(15, 15));
  resizeTriangle.setPoint(2, sf::Vector2f(0, 15));
  resizeTriangle.setOrigin(0,0);
  resizeTriangle.setFillColor(sf::Color(75,75,75,255));
  resizeTriangle.setPosition(clientWinSize_x+borderWinSize_left,clientWinSize_y+borderWinSize_up);
  closeButton.setPosition(clientWinSize_x,1.f);
  closeButton.linkTo(&onClose);
  closeButton.setOutlineThickness(0);
  closeButton.setClickedColor(sf::Color(142,42,42,255));
  closeButton.setIdleColor(sf::Color(100,42,42,255));
  sf::RenderWindow window(sf::VideoMode(clientWinSize_x+borderWinSize_right+borderWinSize_left,
                                        clientWinSize_y+borderWinSize_up+borderWinSize_down), 
                                        "Millenium Synthesizer",0);
   
                                       
  sf::View winView(sf::FloatRect(0,0,window.getSize().x,window.getSize().y));                                     
  float viewPortMin_x=borderWinSize_left/(float)window.getSize().x;
  float viewPortMin_y=borderWinSize_up/(float)window.getSize().y;
  float viewPortMax_x=clientWinSize_x/(float)window.getSize().x;
  float viewPortMax_y=clientWinSize_y/(float)window.getSize().y;
  sf::Text winTitle("Millenium Synthesizer",globalfont,11);
  winTitle.setPosition(borderWinSize_left,5.f);
  
  sf::Texture backTexture;
  if (GetSettingsFor("GUI/Background",true))
  {
    backTexture.loadFromFile(GetSettingsFor("GUI/BackgroundImage",std::string("img/background.png")));
  }
  
  sf::Sprite backSprite(backTexture);
  backSprite.setOrigin(2048,0);
  //backSprite.setOrigin(205,423);
  backSprite.setPosition(window.getSize().x+1347 - 1347*window.getSize().x/2048,
                        420*window.getSize().y/(float)1024-500);
  window.setFramerateLimit(Signal::refreshRate);
  
  //Current mouse catcher
  MouseCatcher* currentMouseCatcher=NULL;
  Interface* currentInterfaceCatcher=NULL;
  std::vector<Interface*> _interfaces;
  std::vector<MouseCatcher*> _mouseCatchers;
  
  Scope myScope(sf::Vector2f(clientWinSize_x,clientWinSize_y/4));
  _interfaces.push_back(&myScope);
  _mouseCatchers.push_back(&closeButton);
  
  float dt=0.02;
  unsigned int time=0;
  
  if (argc == 2) {
    window.setTitle(sf::String("Millenium Synthesizer - ") + argv[1]);
    winTitle.setString(sf::String("Millenium Synthesizer - ") + argv[1]);
  } 
  
  _interfaces.push_back(myInterface);
  
  myInterface->setViewport(sf::FloatRect(viewPortMin_x,viewPortMin_y,viewPortMax_x,360.f*viewPortMax_y/clientWinSize_y));
  myScope.setViewport(sf::FloatRect(viewPortMin_x,viewPortMin_y+360.f*viewPortMax_y/clientWinSize_y,viewPortMax_x,100.f*viewPortMax_y/clientWinSize_y));

  Signal leftout, rightout;
  bool sendSignalSuccess=true;
  myScope.setSignal(&leftout);
  
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
            float clientWinSize_x=window.getSize().x-borderWinSize_left-borderWinSize_right;
            float clientWinSize_y=window.getSize().y-borderWinSize_up-borderWinSize_down;
            winView = sf::View(sf::FloatRect(0,0,window.getSize().x,window.getSize().y));                                     
            float viewPortMin_x=borderWinSize_left/(float)window.getSize().x;
            float viewPortMin_y=borderWinSize_up/(float)window.getSize().y;
            float viewPortMax_x=clientWinSize_x/(float)window.getSize().x;
            float viewPortMax_y=clientWinSize_y/(float)window.getSize().y;
          
            resizeTriangle.setPosition(clientWinSize_x+borderWinSize_left,clientWinSize_y+borderWinSize_up);
            closeButton.setPosition(clientWinSize_x,1.f);
            // Toute la place est disponible 
            if (clientWinSize_y > myInterface->getIdealSize().y + myScope.getIdealSize().y)
            {
              myInterface->setViewSize(clientWinSize_x,myInterface->getIdealSize().y);
              float y1 = myInterface->getIdealSize().y/(float)clientWinSize_y;
              myInterface->setViewport(sf::FloatRect(viewPortMin_x,viewPortMin_y,viewPortMax_x,y1*viewPortMax_y));
              myScope.setViewSize(clientWinSize_x,myScope.getIdealSize().y);
              float y2 = myScope.getIdealSize().y/(float)clientWinSize_y;
              myScope.setViewport(sf::FloatRect(viewPortMin_x,viewPortMin_y+y1*viewPortMax_y,viewPortMax_x,y2*viewPortMax_y));
              y1+=y2;
            }
            else //on alloue proportionellement à la place dispo
            {
              float tot = myInterface->getIdealSize().y + myScope.getIdealSize().y;
              myInterface->setViewSize(clientWinSize_x,myInterface->getIdealSize().y*clientWinSize_y/tot);
              float y1 = myInterface->getIdealSize().y/tot;
              myInterface->setViewport(sf::FloatRect(viewPortMin_x,viewPortMin_y,viewPortMax_x,y1*viewPortMax_y));
              myScope.setViewSize(clientWinSize_x,myScope.getIdealSize().y*clientWinSize_y/tot);
              float y2 = myScope.getIdealSize().y/tot;
              myScope.setViewport(sf::FloatRect(viewPortMin_x,viewPortMin_y+y1*viewPortMax_y,viewPortMax_x,y2*viewPortMax_y));
              y1+=y2;
            }
            
            backSprite.setPosition(window.getSize().x+1347 - 1347*window.getSize().x/2048,
                                   420*window.getSize().y/(float)1024-500);
          }
          break;
        case sf::Event::MouseButtonPressed:
          if (event.mouseButton.button == sf::Mouse::Left)
          {
            previousMousePos=sf::Vector2i(event.mouseButton.x,event.mouseButton.y);
            previousWinPos=window.getPosition();
            previousWinSize=window.getSize();
            if (!currentMouseCatcher)
            {
              for (unsigned int i =0; i < _interfaces.size(); i++)
              {
                sf::Vector2f v = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x,event.mouseButton.y),_interfaces[i]->getView()); 
                if (currentMouseCatcher = _interfaces[i]->onMousePress(v.x,v.y))
                {
                  currentInterfaceCatcher = _interfaces[i];
                  break;
                }
              }
            }
            if (!currentMouseCatcher)
            {
              for (unsigned int i =0; i < _mouseCatchers.size(); i++)
              {
                sf::Vector2f v = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x,event.mouseButton.y),winView); 
                if (_mouseCatchers[i]->onMousePress(v.x,v.y))
                {
                  currentMouseCatcher=_mouseCatchers[i];
                  currentInterfaceCatcher = 0;
                  break;
                }
              }
            }
            if (!currentMouseCatcher)
            {
              if (event.mouseButton.x > window.getSize().x - borderWinSize_right && 
                  event.mouseButton.y > window.getSize().y - borderWinSize_down)
              {
                onResizeWin=true;
                resizeTriangle.setFillColor(sf::Color(142,142,142,255));
              }
              else
                onMoveWin=true;
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
              currentInterfaceCatcher=NULL;
            }
            else if (currentMouseCatcher)
            {
              sf::Vector2f v = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x,event.mouseButton.y),winView); 
              currentMouseCatcher->onMouseRelease(v.x,v.y);
              currentMouseCatcher=NULL;
              currentInterfaceCatcher=NULL;
            }
            else if (onResizeWin)
            {
              onResizeWin=false;
              resizeTriangle.setFillColor(sf::Color(75,75,75,255));
            }
            else if (onMoveWin)
            {
              onMoveWin=false;
            }
          }
          break;
        case sf::Event::MouseMoved:
          if (currentMouseCatcher && currentInterfaceCatcher)
          {
            sf::Vector2f v = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x,event.mouseMove.y),currentInterfaceCatcher->getView());
            currentMouseCatcher->onMouseMove(v.x,v.y);
          }
          else if (currentMouseCatcher)
          {
            sf::Vector2f v = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x,event.mouseButton.y),winView); 
            currentMouseCatcher->onMouseMove(v.x,v.y);
          }
          else if (onMoveWin)
          {
            window.setPosition(sf::Vector2i(event.mouseMove.x,event.mouseMove.y) - previousMousePos + previousWinPos);
            previousWinPos=window.getPosition();
          }
          else if (onResizeWin)
          {
            if (event.mouseMove.x > 0 && event.mouseMove.y > 0)
            {
              sf::Vector2u newSize = sf::Vector2u(event.mouseMove.x-previousMousePos.x + previousWinSize.x,event.mouseMove.y-previousMousePos.y + previousWinSize.y);
              if (newSize.x < 400) newSize.x = 400;
              if (newSize.y < 200) newSize.y = 200;
              window.setSize(newSize);
            }
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
    
    if (onClose) window.close();
    
    myInterface->update();

    //sendSignalSuccess = as t'on utilisé le verre d'eau ?
    if (sendSignalSuccess) 
    {
      //std::cout << "generating output..." << std::endl;
      time++;
      myInstrument->step(&leftout, &rightout); //le verre d'eau est vide donc on le rempli
      myScope.update();     
      sf::Lock lock(stream);
      //std::cout << "try..." << std::endl;
      sendSignalSuccess = stream.writeStereoSignal(leftout, rightout);//on essai de verser le verre d'eau dans l'entonoir
    }
    else
    {
      //std::cout << "retry..." << std::endl;
      sf::Lock lock(stream);
      sendSignalSuccess = stream.writeStereoSignal(leftout, rightout);
    }
    
   
    
    window.clear(sf::Color(42,42,42,255));
    
    window.setView(winView);
    window.draw(backSprite);
    window.draw(resizeTriangle);
    window.draw(winTitle);
    for (unsigned int i =0; i < _mouseCatchers.size(); i++)
    {
      window.draw(*(_mouseCatchers[i]));
    }
    
    for (unsigned int i =0; i < _interfaces.size(); i++)
    {
      window.setView(_interfaces[i]->getView());
      window.draw(*(_interfaces[i]));
    }
    
    
    // Update the window
    window.display();
    
    //std::cout << "CPU usage : " << BASS_ASIO_GetCPU() << std::endl;
  }
  
  //
  delete driver;
  //r.close();
  return 0;
}
