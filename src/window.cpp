
/****************************************************************************
Nom ......... : window.cpp
Role ........ : Defini une fenetre qui utilise des interfaces
Auteur ...... : Julien DE LOOR
Version ..... : V1.7 olol
Licence ..... : © Copydown™
****************************************************************************/

#ifdef _MSC_VER
#pragma warning( disable : 4244 ) //"Possible perte de données"
#endif

#include <iostream>
#include "window.hpp"
#include "preset.hpp"

extern sf::Font globalfont;
NEWindow::NEWindow(sf::VideoMode mode, 
                   const sf::String& title,
                   const sf::ContextSettings& settings) :
#ifdef COMPILE_WINDOWS
sf::RenderWindow(mode,title,0,settings),
#else
sf::RenderWindow(mode,title,sf::Style::Default,settings),
#endif
_borderSizeUp(40),
_borderSizeDown(20),
_borderSizeRight(20),
_borderSizeLeft(20),
_clientSize(mode.width-_borderSizeRight-_borderSizeLeft,
            mode.height-_borderSizeUp-_borderSizeDown),
_viewportMin((float)_borderSizeLeft/(float)mode.width,
             (float)_borderSizeUp/(float)mode.width),
_viewportMax((float)_clientSize.x/(float)mode.width,
             (float)_clientSize.y/(float)mode.width),
_onMoveWin(false),
_onResizeWin(false),
_onClose(0),
#ifdef COMPILE_WINDOWS
_closeButton(sf::Vector2f(_borderSizeRight+_borderSizeLeft,
                          _borderSizeUp*0.5f),"X"),
#endif
_title(title,globalfont,11),
_currentInterfaceCatcher(NULL),
_currentMouseCatcher(NULL),
_fullView(sf::FloatRect(0,0,mode.width,mode.height)),
_backSprite(),
_backTexture(),
_backCenter(0,0)         
{
#ifdef COMPILE_WINDOWS
  //Close Button params
  _closeButton.linkTo(&_onClose,ButtonMode::on);
  _closeButton.setOutlineThickness(0);
  _closeButton.setClickedColor(sf::Color(142,42,42,255));
  _closeButton.setIdleColor(sf::Color(100,42,42,255));

  
  //ResizeTriangle
  _resizeTriangle.setPointCount(3);
  _resizeTriangle.setPoint(0, sf::Vector2f(15, 0));
  _resizeTriangle.setPoint(1, sf::Vector2f(15, 15));
  _resizeTriangle.setPoint(2, sf::Vector2f(0, 15));
  _resizeTriangle.setOrigin(0,0);
  _resizeTriangle.setFillColor(sf::Color(75,75,75,255));

  registerMouseCatcher(_closeButton);
#endif 
  arrange();
  
  
}

NEWindow::~NEWindow(){
  

}

bool NEWindow::useEvent(const sf::Event& event)
{
  switch (event.type)
  {
    case sf::Event::Closed:
      close();
      return true;
    case sf::Event::Resized:
      arrange();
      return true;
      
    ///Gestion de la capture de la souris pour les Mouse Catchers
    case sf::Event::MouseButtonPressed:
      if (event.mouseButton.button == sf::Mouse::Left) 
      {
        const sf::Vector2i mousePosition(event.mouseButton.x,
                                         event.mouseButton.y);
        _previousMousePos=mousePosition;
        _previousWinPos=getPosition();
        _previousWinSize=getSize();
        if (!_currentMouseCatcher) //Priorité aux interfaces
        {
          for (unsigned int i =0; i < _interfaces.size(); i++)
          {
            if (!_interfaces[i]->onIt(event.mouseButton.x,
                                      event.mouseButton.y,
                                      getSize().x,
                                      getSize().y
                                      )) continue;
          
            sf::Vector2f v = mapPixelToCoords(mousePosition,
                                              _interfaces[i]->getView()); 
            if ((_currentMouseCatcher = _interfaces[i]->onMousePress(v.x,v.y)))
            {
              _currentInterfaceCatcher = _interfaces[i];
              return true;
            }
          }
        }
        sf::Vector2f v = mapPixelToCoords(mousePosition,_fullView); 
        if (!_currentMouseCatcher) //ensuite élèments de la fenetre
        {
          for (unsigned int i =0; i < _mouseCatchers.size(); i++)
          {
            if (_mouseCatchers[i]->onMousePress(v.x,v.y))
            {
              _currentMouseCatcher=_mouseCatchers[i];
              _currentInterfaceCatcher = 0;
              return true;
            }
          }
        }
#ifdef COMPILE_WINDOWS
        if (!_currentMouseCatcher) //enfins éléments spéciaux
        {
          if (mousePosition.x > getSize().x - _borderSizeRight && 
              mousePosition.y > getSize().y - _borderSizeDown)
          {
            _onResizeWin=true;
            _resizeTriangle.setFillColor(sf::Color(142,142,142,255));
            
          }
          else
            _onMoveWin=true;
          return true;
        }
#endif
      }
      break;
    case sf::Event::MouseButtonReleased:
      if (event.mouseButton.button == sf::Mouse::Left)
      {
        const sf::Vector2i mousePosition(event.mouseButton.x,
                                         event.mouseButton.y);
        if (_currentMouseCatcher && _currentInterfaceCatcher)
        {
          sf::Vector2f v = mapPixelToCoords(mousePosition,
                                       _currentInterfaceCatcher->getView()); 
          _currentMouseCatcher->onMouseRelease(v.x,v.y);
          _currentMouseCatcher=NULL;
          _currentInterfaceCatcher=NULL;
        }
        else if (_currentMouseCatcher)
        {
          sf::Vector2f v = mapPixelToCoords(mousePosition,_fullView); 
          _currentMouseCatcher->onMouseRelease(v.x,v.y);
          _currentMouseCatcher=NULL;
          _currentInterfaceCatcher=NULL;
        }
#ifdef COMPILE_WINDOWS
        else if (_onResizeWin)
        {
          _onResizeWin=false;
          _resizeTriangle.setFillColor(sf::Color(75,75,75,255));
        }
        else if (_onMoveWin)
        {
          _onMoveWin=false;
        }
#endif
        else return false;
        checkInterrupt();
        return true;
      }
      break;
    case sf::Event::MouseMoved:
      {
        const sf::Vector2i mousePosition(event.mouseMove.x,
                                         event.mouseMove.y);
        if (_currentMouseCatcher && _currentInterfaceCatcher)
        {
          sf::Vector2f v = mapPixelToCoords(mousePosition,
                                         _currentInterfaceCatcher->getView());
          _currentMouseCatcher->onMouseMove(v.x,v.y);
        }
        else if (_currentMouseCatcher)
        {
          sf::Vector2f v = mapPixelToCoords(mousePosition,_fullView); 
          _currentMouseCatcher->onMouseMove(v.x,v.y);
        }
#ifdef COMPILE_WINDOWS
        else if (_onMoveWin)
        {
          setPosition(mousePosition-_previousMousePos+_previousWinPos);
          _previousWinPos=getPosition();
        }
        else if (_onResizeWin)
        {
          if (mousePosition.x > 0 && mousePosition.y > 0)
          {
            sf::Vector2u newSize(mousePosition.x-_previousMousePos.x + _previousWinSize.x,
                                 mousePosition.y-_previousMousePos.y + _previousWinSize.y);
            if (newSize.x < 400) newSize.x = 400;
            if (newSize.y < 200) newSize.y = 200;
            setSize(newSize);
            arrange();
          }
        }
#endif
        else return false;
        checkInterrupt();
        return true;
      }
      
    case sf::Event::KeyPressed:
      break;
    case sf::Event::KeyReleased:
      break;
    default:
      break;
  }
  
  return false;
}

void NEWindow::arrange() {
  _title.setPosition(_borderSizeLeft,5.f);
  
  _clientSize.x=getSize().x-_borderSizeLeft-_borderSizeRight;
  _clientSize.y=getSize().y-_borderSizeUp-_borderSizeDown;
  _fullView = sf::View(sf::FloatRect(0,0,getSize().x,getSize().y));
  
  _viewportMin.x=_borderSizeLeft/(float)getSize().x;
  _viewportMin.y=_borderSizeUp/(float)getSize().y;
  _viewportMax.x=_clientSize.x/(float)getSize().x;
  _viewportMax.y=_clientSize.y/(float)getSize().y;
#ifdef COMPILE_WINDOWS
  _resizeTriangle.setPosition(_clientSize.x+_borderSizeLeft,
                              _clientSize.y+_borderSizeUp);
  _closeButton.setPosition(_clientSize.x-5,5);
#endif

   _backSprite.setPosition(-(int)_backCenter.x+(int)_backCenter.x*(int)getSize().x/((int)_backTexture.getSize().x+1),
                           (int)_backCenter.y*(int)getSize().y/(float)(_backTexture.getSize().y+1)-(int)_backCenter.y);

  // Toute la place est disponible 
  sf::Vector2u idealSize(0,0);
  for (unsigned int k=0;k<_interfaces.size();k++)
    idealSize+=_interfaces[k]->getIdealSize();
  
  if (_clientSize.y >= idealSize.y) //Plus de place que necessaire
  {
    float y1=0;
    for (unsigned int i=0;i<_interfaces.size();i++)
    {
      float sfac=_clientSize.y/(float)idealSize.y;
      _interfaces[i]->setViewSize(_clientSize.x/sfac,_interfaces[i]->getIdealSize().y);
      float y2 = sfac*_interfaces[i]->getIdealSize().y/(float)_clientSize.y;
      _interfaces[i]->setViewport(sf::FloatRect(_viewportMin.x,
                                      _viewportMin.y+y1*_viewportMax.y,
                                      _viewportMax.x,
                                      y2*_viewportMax.y));
      y1+=y2;
    }
  }
  else //on alloue proportionellement à la place dispo
  {
    float y1=0;
    for (unsigned int i=0;i<_interfaces.size();i++)
    {
      _interfaces[i]->setViewSize(_clientSize.x,
                                  _interfaces[i]->getIdealSize().y*_clientSize.y/(float)idealSize.y);
      float y2 = _interfaces[i]->getIdealSize().y/(float)idealSize.y;
      _interfaces[i]->setViewport(sf::FloatRect(_viewportMin.x,
                                                _viewportMin.y+y1*_viewportMax.y,
                                                _viewportMax.x,
                                                y2*_viewportMax.y));
      y1+=y2;
    }
  }
}

void NEWindow::drawContent()
{
  setView(_fullView);
  draw(_backSprite);
#ifdef COMPILE_WINDOWS
  draw(_title);
  draw(_resizeTriangle);
#endif
  for (unsigned int i =0; i < _mouseCatchers.size(); i++)
  {
    draw(*(_mouseCatchers[i]));
  }
  
  //On dessine Les interfaces
  for (unsigned int i =0; i < _interfaces.size(); i++)
  {
    setView(_interfaces[i]->getView());
    draw(*(_interfaces[i]));
  }
}

bool NEWindow::checkInterrupt()
{
  if (_onClose) {
    close();
    _onClose=false;
    return true;
  }
  return false;
}


void NEWindow::setBackgroundTexture(const std::string& name,
                                    const sf::Vector2i center)
{
   _backCenter=center;
   _backTexture.loadFromFile(name);
   _backSprite.setTexture(_backTexture,true);
   _backSprite.setPosition(-(int)_backCenter.x+(int)_backCenter.x*(int)getSize().x/((int)_backTexture.getSize().x+1),
                           (int)_backCenter.y*(int)getSize().y/(float)(_backTexture.getSize().y+1)-(int)_backCenter.y);
}

extern void panic();

TrackControlBar::TrackControlBar( const sf::Vector2f &size) :
Interface(sf::Vector2u(BUTTON_WIDTH*10,BUTTON_HEIGHT+10),size),
_buttonTexture(),
_playButton(sf::Vector2f(BUTTON_WIDTH,BUTTON_HEIGHT),sf::String(),ButtonMode::toggle),
_recordButton(sf::Vector2f(BUTTON_WIDTH,BUTTON_HEIGHT),sf::String(),ButtonMode::toggle),
_rewindButton(sf::Vector2f(BUTTON_WIDTH,BUTTON_HEIGHT),sf::String()),
_loadMIDIButton(sf::Vector2f(BUTTON_WIDTH,BUTTON_HEIGHT),sf::String()),
_saveMIDIButton(sf::Vector2f(BUTTON_WIDTH,BUTTON_HEIGHT),sf::String()),
_loadPresetButton(sf::Vector2f(BUTTON_WIDTH,BUTTON_HEIGHT),sf::String()),
_savePresetButton(sf::Vector2f(BUTTON_WIDTH,BUTTON_HEIGHT),sf::String()),
_panicButton(sf::Vector2f(BUTTON_WIDTH,BUTTON_HEIGHT),sf::String())
{
  _buttonTexture.loadFromFile("img/button.png");
  _playButton.setTexture(_buttonTexture,
                         sf::IntRect(0,BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT),
                         sf::IntRect(BUTTON_WIDTH, BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT));                        
  _playButton.setPosition(0,0);
  
  _recordButton.setTexture(_buttonTexture,
                           sf::IntRect(0,0,BUTTON_WIDTH,BUTTON_HEIGHT),
                           sf::IntRect(BUTTON_WIDTH,0, BUTTON_WIDTH, BUTTON_HEIGHT));  
  _recordButton.setPosition(BUTTON_WIDTH,0);
  
  
  _rewindButton.setTexture(_buttonTexture,
                           sf::IntRect(0,2*BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT),
                           sf::IntRect(BUTTON_WIDTH, 2*BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT));                        
  _rewindButton.setPosition(2*BUTTON_WIDTH,0);
  
  _loadMIDIButton.setTexture(_buttonTexture,
                             sf::IntRect(0,5*BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT),
                             sf::IntRect(BUTTON_WIDTH, 5*BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT));                   
  _loadMIDIButton.setPosition(3*BUTTON_WIDTH,0);
  
  
  _saveMIDIButton.setPosition(4*BUTTON_WIDTH,0);
  _saveMIDIButton.setTexture(_buttonTexture,
                             sf::IntRect(0,4*BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT),
                             sf::IntRect(BUTTON_WIDTH, 4*BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT));
  
  _loadPresetButton.setPosition(5*BUTTON_WIDTH,0);
  _loadPresetButton.setTexture(_buttonTexture,
                               sf::IntRect(0,6*BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT),
                               sf::IntRect(BUTTON_WIDTH, 6*BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT));
  
  _savePresetButton.setPosition(6*BUTTON_WIDTH,0);
  _savePresetButton.setTexture(_buttonTexture,
                               sf::IntRect(0,3*BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT),
                               sf::IntRect(BUTTON_WIDTH, 3*BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT));
  _panicButton.setPosition(7*BUTTON_WIDTH,0);
  _panicButton.setTexture(_buttonTexture,
                          sf::IntRect(0,7*BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT),
                          sf::IntRect(BUTTON_WIDTH, 7*BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT));
  //_panicButton.setProcess(panic);
  addMouseCatcher(&_playButton);
  addMouseCatcher(&_recordButton);
  addMouseCatcher(&_rewindButton);
  addMouseCatcher(&_loadMIDIButton);
  addMouseCatcher(&_saveMIDIButton);
  addMouseCatcher(&_loadPresetButton);
  addMouseCatcher(&_savePresetButton);
  addMouseCatcher(&_panicButton);
}


void TrackControlBar::setTrack(Track* t){
  _rewindButton.setProcess(RewindTrackRoutine,t);
  _loadMIDIButton.setProcess(OpenFromMIDIFileRoutine,t);
  _saveMIDIButton.setProcess(SaveTrackToMIDIFileRoutine,t);
}

void TrackControlBar::setInstrument(AbstractInstrument* i){
  _loadPresetButton.setProcess(LoadInstrumentPresetRoutine,i);
  _savePresetButton.setProcess(SaveInstrumentPresetRoutine,i);
}

TrackControlBar::~TrackControlBar()
{
}

void TrackControlBar::setRecordState(int* rs) 
{
  _recordButton.linkTo(rs);
}

void TrackControlBar::setPlayingState(int* ps)
{
  _playButton.linkTo(ps);
}

