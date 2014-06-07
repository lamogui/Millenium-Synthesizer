
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
#include "scope.hpp"
#include "window.hpp"

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
  unsigned int streamSize=Signal::size*6; //4 fois la taille d'un signal
  AudioStream stream(streamSize);
  //initialisation des signaux
  Signal leftout, rightout;
  //On remplis le stream avec du silence
  while (stream.writeStereoSignal(leftout, rightout));
  //Acquitement
  bool sendSignalSuccess=true;
  //on demmarre la lecture du stream
  if (!driver->start(&stream)) return 0xdead; //si elle échoue : tant pis
  
  
  ///Paramètres de la fenêtre
  //configuration des fonts 
  globalfont.loadFromFile("fonts/unispace rg.ttf");
  
  ///Création de la fenêtre
  NEWindow window(sf::VideoMode(942,600),"Millenium Synthesizer");
  
  ///Initialisation de l'instrument
  AbstractInstrument* myInstrument=NULL;  //L'instrument    
  Interface* myInterface=NULL;            //Son interface
  if (argc == 2)
  {
    if (std::string("puresquare") == argv[1]) 
    {
      //La taille est inferieur à la taille de l'oscilloscope
      myInstrument = new PureSquare;
      myInterface = new PureSquareInterface((PureSquare*) myInstrument,
                                            sf::Vector2f(window.getSize().x,360));
    }
    else 
    {
      myInstrument = new NELead6;
      myInterface = new NELead6Interface((NELead6*) myInstrument,
                                          sf::Vector2f(window.getSize().x,360)); 
    }
  }
  else 
  {
    myInstrument = new NELead6;
    myInterface = new NELead6Interface((NELead6*) myInstrument,
                                        sf::Vector2f(window.getSize().x,360));
  }
  
  
  
  
  ///Textures
  //texture des boutons
  sf::Texture buttonTexture;
  buttonTexture.loadFromFile("img/button.png");
  //Texture de fond
  sf::Texture backTexture;
  if (GetSettingsFor("GUI/Background",true))
  {
    window.setBackgroundTexture(GetSettingsFor("GUI/BackgroundImage",
                                std::string("img/background.png")),
                                sf::Vector2i(600,500));
  }

  //Piste
  Track myTrack(myInstrument);
  int playing=0;
  int recording=0;
  //Controleur de piste
  TrackControlBar trackControlBar(sf::Vector2f(window.getSize().x,BUTTON_HEIGHT+10));
  trackControlBar.setTrack(&myTrack);
  trackControlBar.setInstrument(myInstrument);
  trackControlBar.setRecordState(&recording);
  trackControlBar.setPlayingState(&playing);
  
  
  //Oscilloscope
  Scope myScope(sf::Vector2f(window.clientSize().x,100));
  myScope.setSignal(&leftout);
  myScope.setColor(myInterface->getColor());
  
  //window.registerInterface(*myMenuBar);
  window.registerInterface(trackControlBar);
  window.registerInterface(*myInterface);
  window.registerInterface(myScope);
  window.arrange();
  
  ///Gestionnaire de Notes
  std::map<sf::Keyboard::Key,Note*> notes;
  while (window.isOpen())
  {
    ///Gestion des evenements
    sf::Event event;
    while (window.pollEvent(event))
    {
      switch (event.type)
      {
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
                if (recording) 
                  notes[event.key.code] = myTrack.recordNoteStart(id,1.0);
                else
                  notes[event.key.code] = new Note(0,id,1.0);
                myInstrument->playNote(*notes[event.key.code]);
              }  
            }
          }
          break;
        case sf::Event::KeyReleased:
          if (notes.find(event.key.code) != notes.end())
          {
            if (recording || notes[event.key.code]->start())
               myTrack.recordNoteRelease(notes[event.key.code]);
               //équivalent à
               //notes[event.key.code]->length= track.time() - notes[event.key.code]->start;

            notes[event.key.code]->sendStopSignal();
            
            if (!notes[event.key.code]->start()) 
               delete notes[event.key.code];
            notes.erase(event.key.code);
          }
          break;
        default:
          window.useEvent(event);
          break;
      }
   }
    
    //Mise à jour de l'interface
    myInterface->update();
    
    
    if (recording) trackControlBar.playButton().forceOn();
    if (playing)
      myTrack.tick();
    
    //le verre d'eau est vide donc on le rempli
    myInstrument->step(&leftout, &rightout); 
    
    
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
    
    //Mise à jour de l'oscillo
    myScope.update();
    
   ///Dessin  !!! 
    window.clear(sf::Color(42,42,42,255)); //on efface
    window.drawContent();
    //Mise à jour réele à l'écran. + limitation du framerate
    window.display();
    
    //std::cout << "CPU usage : " << BASS_ASIO_GetCPU() << std::endl;
  }
  
  driver->stop();
  driver->free();
  
  //Nettoyage
  myTrack.setInstrument(NULL);
  delete myInterface;
  delete myInstrument;
  delete driver;
  return 0;
}
