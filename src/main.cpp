
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
#include "careme.hpp"
#include "scope.hpp"
#include "track.hpp"
#include "cado.hpp"
#include "midi.hpp"
#include "file.hpp"
#include "preset.hpp"
 
sf::Font globalfont; 

int main(int argc, char** argv)
{
  /*//on realise une fft de 9 point d'un sinus
  FFT fft(9);
  //on remplie le sample s de 9 points d'un premier lobe de sinus
  //le sinus est de frequence f=16Hz
  //fe=128Hz
  sample s[9]={0};
  for (int i=0; i<9; i++) {
    s[i]=sin(M_PI*(float)i/(float)8);
  }
  //on calcul la fft
  fft.compute(s, 9);*/

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
  
  
  ///Paramètres de la fenêtre
  //Tailles des bordures
  float borderWinSize_up=40;
  float borderWinSize_down=20;
  float borderWinSize_right=20;
  float borderWinSize_left=20;
  float borderButtonBar_left=34;
  //Taille alloué pour les interfaces
  float clientWinSize_x=Signal::size/*+205-40*/; 
  float clientWinSize_y=360+100+borderButtonBar_left;
  //Variables de gestion de l'état de la fenêtre
  #ifdef COMPILE_WINDOWS
  bool onMoveWin=false;
  bool onResizeWin=false;
  #endif
  int onClose=0;
  sf::Vector2i previousWinPos;
  sf::Vector2i previousMousePos;
  sf::Vector2u previousWinSize;
  //configuration des fonts 
  globalfont.loadFromFile("fonts/unispace rg.ttf");
  
  ///Initialisation de l'instrument
  AbstractInstrument* myInstrument=NULL;  //L'instrument    
  Interface* myInterface=NULL;            //Son interface
  if (argc >= 2)
  {
    if (std::string("careme") == argv[1]) 
    {
      myInstrument = new Careme;
      myInterface = new CaremeInterface((Careme*) myInstrument,
                                        sf::Vector2f(clientWinSize_x-borderButtonBar_left,360));
    }
    else if (std::string("puresquare") == argv[1]) 
    {
      //La taille est inferieur à la taille de l'oscilloscope
      clientWinSize_x=720+borderButtonBar_left; 
      myInstrument = new PureSquare;
      myInterface = new PureSquareInterface((PureSquare*) myInstrument,
                                            sf::Vector2f(clientWinSize_x-borderButtonBar_left,360));
    }
	else if (std::string("cado") == argv[1]) 
    {
      //La taille est inferieur à la taille de l'oscilloscope
      clientWinSize_x=600+borderButtonBar_left; 
      myInstrument = new Cado;
      myInterface = new CadoInterface((Cado*) myInstrument,
                                            sf::Vector2f(clientWinSize_x-borderButtonBar_left,360));
    }
    else 
    {
      myInstrument = new NELead6;
      myInterface = new NELead6Interface((NELead6*) myInstrument,
                                          sf::Vector2f(clientWinSize_x-borderButtonBar_left,360)); 
    }
  }
  else 
  {
    myInstrument = new NELead6;
    myInterface = new NELead6Interface((NELead6*) myInstrument,
                                        sf::Vector2f(clientWinSize_x-borderButtonBar_left,360));
  }
  
  FILE* f=fopen("preset.prst","rb");
  if (f) {
     Preset p;
     unsigned int fs = fsize(f);
     std::cout << "Preset file size " << fs << std::endl;
     unsigned char* buf=(unsigned char*)malloc(fs);
     fread((void*) buf,1,fs,f);
     p.read_from_buffer(buf,fs);
     p.unpack(myInstrument);
     fclose(f);
  }
  ///Initialisation de la piste d'enregistrement
  Track myTrack(myInstrument);
  
  if (argc >= 3) {
    FILE* file = fopen(argv[2], "rb");
    if (file)
    {
      int filesize = fsize(file);
      unsigned char* buffer= (unsigned char*) malloc(filesize);
      fread(buffer,1,filesize,file);
      Midi_head head(1, 0, 25, 2);
      if (head.read_from_buffer(buffer, filesize))
      {
        std::cout << "File " << argv[2] << " infos" << std::endl;
        std::cout << "Header (" << Midi_head::size << " bytes):" << std::endl;
        head.print_infos();
        if (head.format() == 1)
        {        
          Midi_track0 track0;
          unsigned int track0_len;
          if (track0_len = track0.read_from_buffer(buffer+Midi_head::size, filesize-Midi_head::size))
          {
            std::cout << "Track 0 (" << track0_len << " bytes):"<< std::endl;
            track0.print_infos();
            unsigned int delta = Midi_head::size + track0_len;
            Midi_track track(head);
            Track tempTrack;
            unsigned int track_len;
            unsigned count=0;
            while (filesize > (int) delta)
            {
              if (track_len=track.read_from_buffer(buffer+delta, filesize-delta))
              {
                tempTrack.importFromMidiTrack(track);
                myTrack.concatenate(tempTrack); 
                delta+=track_len;
                std::cout << "Track " << count + 1 <<  " (" << track_len << " bytes) Time " << tempTrack.fastLength() << " concatenate time " << myTrack.fastLength() << std::endl;
                count++;
              }
              else {
                std::cout << "Failed to read Track " << count + 1 << std::endl;
                break;
              }
            }
            
            std::cout << "Readed " << count + 1 << "/" << head.tracks() << "  with success !" << std::endl;
            std::cout << "Duree  " << myTrack.fastLength() << std::endl;
            
          }
          else 
          {
            std::cout << "Error: " << argv[2] << " failed to load track 0" << std::endl;
          } 
        }
        else if (head.format() == 0)
        {
           Midi_track track(head);
           unsigned int track_len;
           if (track_len = track.read_from_buffer(buffer+Midi_head::size, filesize-Midi_head::size))
           {
             myTrack.importFromMidiTrack(track);
             std::cout << "Track 0 (" << track_len << " bytes) Time " << myTrack.fastLength() << std::endl;
           }
           else std::cout << "Failed to read MIDI 0 " << std::endl;
        }
        
      }
      else
      {
        std::cout << "Error: " << argv[2] << " is not a compatible midi file" << std::endl; 
      }
      fclose(file);
    }
    else {
      std::cout << "Unable to open: " << argv[2] << std::endl; 
    }
  }
  
  ///Création de la fenêtre
  sf::VideoMode video(clientWinSize_x+borderWinSize_right+borderWinSize_left,
                      clientWinSize_y+borderWinSize_up+borderWinSize_down);
  #ifdef COMPILE_WINDOWS
  sf::RenderWindow window(video,"Millenium Synthesizer",0);
  #else
  sf::RenderWindow window(video,"Millenium Synthesizer");
  #endif
  //window.setFramerateLimit(Signal::refreshRate);
  
  ///Création des éléments qui composent la fenêtre
  //Bouton de fermeture de la fenetre
  #ifdef COMPILE_WINDOWS
  Button closeButton(sf::Vector2f(borderWinSize_right+borderWinSize_left,
                                  borderWinSize_up*0.5f),"X");
  closeButton.setPosition(clientWinSize_x,0);
  closeButton.linkTo(&onClose);
  closeButton.setOutlineThickness(0);
  closeButton.setClickedColor(sf::Color(142,42,42,255));
  closeButton.setIdleColor(sf::Color(100,42,42,255));
  #endif
  
  //texture des boutons
  sf::Texture buttonTexture;
  buttonTexture.loadFromFile("img/button.png");
  
  //Bouton play
  int playState=0;
  Button playButton(&playState, buttonTexture, 
                                sf::IntRect(0,22,30,22),
                                sf::IntRect(30, 22, 30, 22));
  playButton.setPosition(borderWinSize_left,borderWinSize_up);
  //Bouton enregistrement
  int recordState=0;
  Button recordButton(&recordState, buttonTexture, 
                                    sf::IntRect(0,0,30,22),
                                    sf::IntRect(30, 0, 30, 22));
  recordButton.setPosition(borderWinSize_left,borderWinSize_up+22);

  //Bouton retour au debut
  int rewindState=0;
  Button rewindButton(&rewindState, buttonTexture, 
                                    sf::IntRect(0,44,30,22),
                                    sf::IntRect(30, 44, 30, 22),
                                    ButtonMode::interrupt);
  rewindButton.setPosition(borderWinSize_left,borderWinSize_up+44);
  
  //Triangle de redimensionnement
  #ifdef COMPILE_WINDOWS
  sf::ConvexShape resizeTriangle;
  resizeTriangle.setPointCount(3);
  resizeTriangle.setPoint(0, sf::Vector2f(15, 0));
  resizeTriangle.setPoint(1, sf::Vector2f(15, 15));
  resizeTriangle.setPoint(2, sf::Vector2f(0, 15));
  resizeTriangle.setOrigin(0,0);
  resizeTriangle.setFillColor(sf::Color(75,75,75,255));
  resizeTriangle.setPosition(clientWinSize_x+borderWinSize_left,
                             clientWinSize_y+borderWinSize_up);
  
  #endif
  //Titre de la fenêtre
  sf::Text winTitle("Millenium Synthesizer",globalfont,11);
  winTitle.setPosition(borderWinSize_left,5.f);
  if (argc >= 2) {
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
  Scope myScope(sf::Vector2f(clientWinSize_x,clientWinSize_y/4),true);
  myScope.setSignal(&leftout);
  myScope.setFadeColor(sf::Color(0, 0, 255), sf::Color(255,0,0), 1);
  
  ///Vue et viewports
  //vue de la fenetre entière                       
  sf::View winView(sf::FloatRect(0,0,window.getSize().x,window.getSize().y)); 
  //variables de gestion de gestion des viewport
  float viewPortMin_x=borderWinSize_left/(float)window.getSize().x;
  float viewPortMin_y=borderWinSize_up/(float)window.getSize().y;
  float viewPortMax_x=clientWinSize_x/(float)window.getSize().x;
  float viewPortMax_y=clientWinSize_y/(float)window.getSize().y;
  float viewPortButtonBar_left=borderButtonBar_left/(float)window.getSize().x;
  //Réglages des viewports
  myInterface->setViewport(sf::FloatRect(viewPortMin_x+viewPortButtonBar_left,
                                         viewPortMin_y,
                                         viewPortMax_x-viewPortButtonBar_left,
                                         360.f*viewPortMax_y/clientWinSize_y));
  myScope.setViewport(sf::FloatRect(viewPortMin_x+viewPortButtonBar_left,
                                    viewPortMin_y+360.f*viewPortMax_y/clientWinSize_y,
                                    viewPortMax_x-viewPortButtonBar_left,
                                    100.f*viewPortMax_y/clientWinSize_y));
  
  ///Gestionnaire de Notes
  std::map<sf::Keyboard::Key,Note*> notes;
  float dt=1.f/(float)Signal::refreshRate; 
  //unsigned int time=0; //Temps / dt (entier)
   
  ///Variables de gestion des interfaces/mouseCatchers
  //Interfaces
  Interface* currentInterfaceCatcher=NULL;
  std::vector<Interface*> _interfaces; 
  _interfaces.push_back(myInterface);
  _interfaces.push_back(&myScope);
  //MouseCatchers (exterieurs au interfaces)
  MouseCatcher* currentMouseCatcher=NULL;
  std::vector<MouseCatcher*> _mouseCatchers; 
  #ifdef COMPILE_WINDOWS
  _mouseCatchers.push_back(&closeButton);
  #endif
  _mouseCatchers.push_back(&playButton);
  _mouseCatchers.push_back(&recordButton);
  _mouseCatchers.push_back(&rewindButton);
  
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
        ///Resizing
        case sf::Event::Resized:
          {
            clientWinSize_x=window.getSize().x-borderWinSize_left-borderWinSize_right;
            clientWinSize_y=window.getSize().y-borderWinSize_up-borderWinSize_down;
            winView = sf::View(sf::FloatRect(0,0,window.getSize().x,window.getSize().y));                                     
            viewPortMin_x=borderWinSize_left/(float)window.getSize().x;
            viewPortMin_y=borderWinSize_up/(float)window.getSize().y;
            viewPortMax_x=clientWinSize_x/(float)window.getSize().x;
            viewPortMax_y=clientWinSize_y/(float)window.getSize().y;
            viewPortButtonBar_left=borderButtonBar_left/(float)window.getSize().x;
#ifdef COMPILE_WINDOWS
            resizeTriangle.setPosition(clientWinSize_x+borderWinSize_left,clientWinSize_y+borderWinSize_up);
            closeButton.setPosition(clientWinSize_x,0);
#endif
            playButton.setPosition(borderWinSize_left,borderWinSize_up);
            recordButton.setPosition(borderWinSize_left,borderWinSize_up+22);
            // Toute la place est disponible 
            if (clientWinSize_y > myInterface->getIdealSize().y + myScope.getIdealSize().y)
            {
              myInterface->setViewSize(clientWinSize_x,myInterface->getIdealSize().y);
              float y1 = myInterface->getIdealSize().y/(float)clientWinSize_y;
              myInterface->setViewport(sf::FloatRect(viewPortMin_x+viewPortButtonBar_left,
                                                     viewPortMin_y,
                                                     viewPortMax_x-viewPortButtonBar_left,
                                                     y1*viewPortMax_y));
              myScope.setViewSize(clientWinSize_x,myScope.getIdealSize().y);
              float y2 = myScope.getIdealSize().y/(float)clientWinSize_y;
              myScope.setViewport(sf::FloatRect(viewPortMin_x+viewPortButtonBar_left,
                                                viewPortMin_y+y1*viewPortMax_y,
                                                viewPortMax_x-viewPortButtonBar_left,
                                                y2*viewPortMax_y));
              y1+=y2;
            }
            else //on alloue proportionellement à la place dispo
            {
              float tot = myInterface->getIdealSize().y + myScope.getIdealSize().y;
              myInterface->setViewSize(clientWinSize_x,myInterface->getIdealSize().y*clientWinSize_y/tot);
              float y1 = myInterface->getIdealSize().y/tot;
              myInterface->setViewport(sf::FloatRect(viewPortMin_x+viewPortButtonBar_left,
                                                     viewPortMin_y,
                                                     viewPortMax_x-viewPortButtonBar_left,
                                                     y1*viewPortMax_y));
              myScope.setViewSize(clientWinSize_x,
                                  myScope.getIdealSize().y*clientWinSize_y/tot);
              float y2 = myScope.getIdealSize().y/tot;
              myScope.setViewport(sf::FloatRect(viewPortMin_x+viewPortButtonBar_left,
                                                viewPortMin_y+y1*viewPortMax_y,
                                                viewPortMax_x-viewPortButtonBar_left,
                                                y2*viewPortMax_y));
              y1+=y2;
            }
            
            backSprite.setPosition(window.getSize().x+1347 - 1347*window.getSize().x/2048,
                                   420*window.getSize().y/(float)1024-500);
          }
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
                if (!_interfaces[i]->onIt(event.mouseButton.x,
                                          event.mouseButton.y,
                                          window.getSize().x,
                                          window.getSize().y
                                          )) continue;
              
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
#ifdef COMPILE_WINDOWS
            if (!currentMouseCatcher) //enfins éléments spéciaux
            {
              if (mousePosition.x > window.getSize().x - borderWinSize_right && 
                  mousePosition.y > window.getSize().y - borderWinSize_down)
              {
                onResizeWin=true;
                resizeTriangle.setFillColor(sf::Color(142,142,142,255));
              }
              else
                onMoveWin=true;
            }
#endif
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
 #ifdef COMPILE_WINDOWS
            else if (onResizeWin)
            {
              onResizeWin=false;
              resizeTriangle.setFillColor(sf::Color(75,75,75,255));
            }
            else if (onMoveWin)
            {
              onMoveWin=false;
            }
#endif
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
#ifdef COMPILE_WINDOWS
          else if (onMoveWin)
          {
            window.setPosition(mousePosition-previousMousePos+previousWinPos);
            previousWinPos=window.getPosition();
          }
          else if (onResizeWin)
          {
            if (mousePosition.x > 0 && mousePosition.y > 0)
            {
              sf::Vector2u newSize(mousePosition.x-previousMousePos.x + previousWinSize.x,
                                   mousePosition.y-previousMousePos.y + previousWinSize.y);
              if (newSize.x < 400) newSize.x = 400;
              if (newSize.y < 200) newSize.y = 200;
              window.setSize(newSize);
            }
          }
#endif
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
                if (recordState) 
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
            if (recordState || notes[event.key.code]->start())
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
          break;
      }
   }
    //Interruption : appuis du bouton quitter
    if (onClose) window.close();
    
    
    if (rewindState) {
      rewindState=0;
      myTrack.seek(0);
    }
    if (recordState) playButton.forceOn();
    
    //Mise à jour de l'interface
    myInterface->update();
    
    //Mise à jour du son
    if (playState)
      myTrack.tick(); //et hop !!!
    //if ((myTrack.time())/((120/60)*4*Signal::refreshRate)) myTrack.seek(0);
    //std::cout << "Mesure : " << (myTrack.time())/((120/60)*Signal::refreshRate) << " battement " << (myTrack.time())/((120/60)*Signal::refreshRate/4)%4 << std::endl;
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
        //sf::sleep(sf::microseconds(m*Signal::size));
	sf::sleep(sf::microseconds(40));
      }
      stream.lock();
      sendSignalSuccess = stream.writeStereoSignal(leftout, rightout);
      stream.unlock();
    } while(!sendSignalSuccess);
    
    //Mise à jour de l'oscillo
    myScope.update();
    
   ///Dessin  !!! 
    window.clear(sf::Color(42,42,42,255)); //on efface
    window.setView(winView);               //On dessine les éléments de la fenetre
    window.draw(backSprite);               //Le fond
    #ifdef COMPILE_WINDOWS
    window.draw(resizeTriangle);           //Triangle de redimensionnement
    #endif
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
  
  driver->stop();
  driver->free();
  
  
  FILE* file=fopen("preset.prst","wb");
  Preset preset;
  preset.pack(myInstrument);
  preset.write_to_file(file);
  fclose(file);
  
  Midi_head head(1,2,25,2);
  Midi_track0 track0;
  Midi_track track(head);
  myTrack.exportToMidiTrack(track);
  
  std::cout << "File size : " << head.size + track0.size() + track.size() << "\n";
   
  file=fopen("test.mid","wb");
  head.write_to_file(file);
  track0.write_to_file(file);
  track.write_to_file(file);
  fclose(file);
  
  //Nettoyage
  myTrack.setInstrument(NULL);
  delete myInterface;
  delete myInstrument;
  delete driver;
  return 0;
}
