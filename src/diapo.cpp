
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
  ///Initialisation de l'aléatoire
  srand(time(NULL));
  
  ///Initialisation du fichier de configuration
  Settings::getInstance().loadFile("settings.ini");
  unsigned int signalFrequency = GetSettingsFor("Signal/Frequency",44100); //fe
  unsigned int signalRefreshRate = GetSettingsFor("Signal/RefreshRate",50);
  Signal::globalConfiguration(signalFrequency,signalRefreshRate);
  
  ///Création du driver audio
  AudioDriver* driver;
  #ifdef COMPILE_WINDOWS //BASSASIO (ONLY ON WINDOWS)
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
  //Le driver ne demmarre pas ? tant pis...
  if (!driver->init(Signal::frequency)) return 0xdead;
  
  ///Initialisation du stream
  unsigned int streamSize=Signal::size<<2; //4 fois la taille d'un signal
  AudioStream stream(streamSize);
  //initialisation des signaux
  Signal leftout, rightout;
  //On remplis le stream avec du silence
  while (stream.writeStereoSignal(leftout, rightout));
  //Acquitement
  bool sendSignalSuccess=true;
  //on demmarre la lecture du stream
  if (!driver->start(&stream)) return 0xdead; //si elle échoue : tant pis
  
  
  sf::VideoMode video=sf::VideoMode::getDesktopMode();
  
  ///Paramètres de la fenêtre
  //Taille alloué pour les interfaces
  float clientWinSize_x=Signal::size/*+205-40*/; 
  float clientWinSize_y=clientWinSize_x*video.height/(float)video.width;//360+100;
  //Tailles des bordures
  float borderWinSize_up=40;
  float borderWinSize_down=20;
  float borderWinSize_right=20;
  float borderWinSize_left=20;
  //Variables de gestion de l'état de la fenêtre
  bool onMoveWin=false;
  bool onResizeWin=false;
  int onClose=0;
  sf::Vector2i previousWinPos;
  sf::Vector2i previousMousePos;
  sf::Vector2u previousWinSize;
  //configuration des fonts 
  globalfont.loadFromFile("fonts/unispace rg.ttf");
  
  ///Initialisation de l'instrument
  AbstractInstrument* myInstrument=NULL;  //L'instrument    
  Interface* myInterface=NULL;            //Son interface
  if (argc == 2)
  {
    if (std::string("careme") == argv[1]) 
    {
      myInstrument = new Careme;
      myInterface = new CaremeInterface((Careme*) myInstrument,
                                        sf::Vector2f(clientWinSize_x,360));
    }
    else if (std::string("puresquare") == argv[1]) 
    {
      //La taille est inferieur à la taille de l'oscilloscope
      clientWinSize_x=720; 
      myInstrument = new PureSquare;
      myInterface = new PureSquareInterface((PureSquare*) myInstrument,
                                            sf::Vector2f(clientWinSize_x,360));
    }
    else 
    {
      myInstrument = new NELead6;
      myInterface = new NELead6Interface((NELead6*) myInstrument,
                                          sf::Vector2f(clientWinSize_x,360)); 
    }
  }
  else 
  {
    myInstrument = new NELead6;
    myInterface = new NELead6Interface((NELead6*) myInstrument,
                                        sf::Vector2f(clientWinSize_x,360));
  }
  
  ///Création de la fenêtre
  sf::RenderWindow window(video,"Millenium Synthesizer",0);
  
  ///Création des éléments qui composent la fenêtre
  //Bouton de fermeture de la fenetre
  Button closeButton(sf::Vector2f(borderWinSize_right+borderWinSize_left,
                                  borderWinSize_up*0.5f),"X");
  closeButton.setPosition(window.getSize().x-borderWinSize_right-borderWinSize_left,0);
  closeButton.linkTo(&onClose);
  closeButton.setOutlineThickness(0);
  closeButton.setClickedColor(sf::Color(142,42,42,255));
  closeButton.setIdleColor(sf::Color(100,42,42,255));
  
  //Titre de la fenêtre
  sf::Text winTitle("Millenium Synthesizer",globalfont,11);
  winTitle.setPosition(borderWinSize_left,5.f);
  if (argc == 2) {
    window.setTitle(sf::String("Millenium Synthesizer - ") + argv[1]);
    winTitle.setString(sf::String("Millenium Synthesizer - ") + argv[1]);
  } 
  
  //Texture de fond
  sf::Texture backTexture;
  if (GetSettingsFor("GUI/Background",true))
  {
    backTexture.loadFromFile(GetSettingsFor("GUI/BackgroundImage",
                             std::string("img/background.png")));
  }
  sf::Sprite backSprite(backTexture);
  backSprite.setOrigin(2048,0);
  //backSprite.setOrigin(205,423);
  backSprite.setPosition(window.getSize().x+1347-1347*window.getSize().x/2048,
                        420*window.getSize().y/(float)1024-500);
  
  //Oscilloscope
  Scope myScope(sf::Vector2f(clientWinSize_x,clientWinSize_y/4));
  myScope.setSignal(&leftout);
  myScope.setColor(myInterface->getColor());
  
  ///Vue et viewports
  //vue de la fenetre entière                       
  sf::View winView(sf::FloatRect(0,0,window.getSize().x,window.getSize().y)); 
  //variables de gestion de gestion des viewport
  float viewPortMin_x=borderWinSize_left/(float)(clientWinSize_x+borderWinSize_left+borderWinSize_right);
  float viewPortMin_y=borderWinSize_up/(float)(clientWinSize_y+borderWinSize_down+borderWinSize_up);
  float viewPortMax_x=clientWinSize_x/(float)(clientWinSize_x+borderWinSize_left+borderWinSize_right);
  float viewPortMax_y=clientWinSize_y/(float)(clientWinSize_y+borderWinSize_down+borderWinSize_up);
  //Réglages des viewports
  myInterface->setViewport(sf::FloatRect(viewPortMin_x,
                                         viewPortMin_y,
                                         viewPortMax_x,
                                         360.f*viewPortMax_y/clientWinSize_y));
  myScope.setViewport(sf::FloatRect(viewPortMin_x,
                                    viewPortMin_y+360.f*viewPortMax_y/clientWinSize_y,
                                    viewPortMax_x,
                                    400.f*viewPortMax_y/clientWinSize_y));
  
  ///Gestionnaire de Notes
  std::map<sf::Keyboard::Key,Note*> notes;
  float dt=1.f/(float)Signal::refreshRate; 
  unsigned int time=0; //Temps / dt (entier)
   
  ///Variables de gestion des interfaces/mouseCatchers
  //Interfaces
  Interface* currentInterfaceCatcher=NULL;
  std::vector<Interface*> _interfaces; 
  _interfaces.push_back(myInterface);
  _interfaces.push_back(&myScope);
  //MouseCatchers (exterieurs au interfaces)
  MouseCatcher* currentMouseCatcher=NULL;
  std::vector<MouseCatcher*> _mouseCatchers; 
  _mouseCatchers.push_back(&closeButton);
  
  while (window.isOpen())
  {
    ///Gestion des evenements
    sf::Event event;
    while (window.pollEvent(event))
    {
      
      switch (event.type)
      {
        case sf::Event::Closed:
          window.close(); // Close window : exit
          break;
        ///Gestion de la capture de la souris pour les Mouse Catchers
        case sf::Event::MouseButtonPressed:
          if (event.mouseButton.button == sf::Mouse::Left) 
          {
            const sf::Vector2i mousePosition(event.mouseButton.x,
                                             event.mouseButton.y);
            previousMousePos=mousePosition;
            previousWinPos=window.getPosition();
            previousWinSize=window.getSize();
            if (!currentMouseCatcher) //Priorité aux interfaces
            {
              for (unsigned int i =0; i < _interfaces.size(); i++)
              {
                sf::Vector2f v = window.mapPixelToCoords(mousePosition,
                                                    _interfaces[i]->getView()); 
                if (currentMouseCatcher = _interfaces[i]->onMousePress(v.x,v.y))
                {
                  currentInterfaceCatcher = _interfaces[i];
                  break;
                }
              }
            }
            if (!currentMouseCatcher) //ensuite élèments de la fenetre
            {
              for (unsigned int i =0; i < _mouseCatchers.size(); i++)
              {
                sf::Vector2f v = window.mapPixelToCoords(mousePosition,winView); 
                if (_mouseCatchers[i]->onMousePress(v.x,v.y))
                {
                  currentMouseCatcher=_mouseCatchers[i];
                  currentInterfaceCatcher = 0;
                  break;
                }
              }
            }
          }
          break;
        case sf::Event::MouseButtonReleased:
          if (event.mouseButton.button == sf::Mouse::Left)
          {
            const sf::Vector2i mousePosition(event.mouseButton.x,
                                             event.mouseButton.y);
            if (currentMouseCatcher && currentInterfaceCatcher)
            {
              sf::Vector2f v = window.mapPixelToCoords(mousePosition,
                                           currentInterfaceCatcher->getView()); 
              currentMouseCatcher->onMouseRelease(v.x,v.y);
              currentMouseCatcher=NULL;
              currentInterfaceCatcher=NULL;
            }
            else if (currentMouseCatcher)
            {
              sf::Vector2f v = window.mapPixelToCoords(mousePosition,winView); 
              currentMouseCatcher->onMouseRelease(v.x,v.y);
              currentMouseCatcher=NULL;
              currentInterfaceCatcher=NULL;
            }
          }
          break;
        case sf::Event::MouseMoved:
          {
          const sf::Vector2i mousePosition(event.mouseMove.x,
                                           event.mouseMove.y);
          if (currentMouseCatcher && currentInterfaceCatcher)
          {
            sf::Vector2f v = window.mapPixelToCoords(mousePosition,
                                           currentInterfaceCatcher->getView());
            currentMouseCatcher->onMouseMove(v.x,v.y);
          }
          else if (currentMouseCatcher)
          {
            sf::Vector2f v = window.mapPixelToCoords(mousePosition,winView); 
            currentMouseCatcher->onMouseMove(v.x,v.y);
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
    //Interruption : appuis du bouton quitter
    if (onClose) window.close();
    
    //Mise à jour de l'interface
    myInterface->update();
    
    //Mise à jour du son
    time++;
      //le verre d'eau est vide donc on le rempli
      myInstrument->step(&leftout, &rightout); 
      //Mise à jour de l'oscillo
      myScope.update();
    
    unsigned l;
    do
    {
      stream.lock();
      l = (stream.getBufferLength() - stream.getAvailableSamplesCount()) >> 1;
      stream.unlock();
      unsigned m=(float)((1.f/(float)Signal::frequency) *  100000.f);
      if (l<Signal::size)
      {
        //std::cout<<"Waiting "<< m*Signal::size << " microseconds\n";
        sf::sleep(sf::microseconds(m*Signal::size));
      }
      stream.lock();
      sendSignalSuccess = stream.writeStereoSignal(leftout, rightout);
      stream.unlock();
    } while(!sendSignalSuccess);
    
   ///Dessin  !!! 
    window.clear(sf::Color(42,42,42,255)); //on efface
    window.setView(winView);               //On dessine les éléments de la fenetre
    window.draw(backSprite);               //Le fond
    window.draw(winTitle);                 //Le titre
    //On dessine tous les élèments propre à la fenêtre (bouton close)
    for (unsigned int i =0; i < _mouseCatchers.size(); i++)
    {
      window.draw(*(_mouseCatchers[i]));
    }
    
    //On dessine Les interfaces
    for (unsigned int i =0; i < _interfaces.size(); i++)
    {
      window.setView(_interfaces[i]->getView());
      window.draw(*(_interfaces[i]));
    }
    
    //Mise à jour réele à l'écran. + limitation du framerate
    window.display();
    
    //std::cout << "CPU usage : " << BASS_ASIO_GetCPU() << std::endl;
  }
  
  //Nettoyage
  delete myInterface;
  delete myInstrument;
  delete driver;
  return 0;
}
