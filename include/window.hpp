

/****************************************************************************
Nom ......... : window.hpp
Role ........ : Defini une fenetre qui utilise des interfaces
Auteur ...... : Julien DE LOOR
Version ..... : V1.0 olol
Licence ..... : © Copydown™
****************************************************************************/


#ifndef __MAINWINDOW
#define __MAINWINDOW
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "interface.hpp"
#include <vector>
#include "config.hpp"
#include "track.hpp"

#define BUTTON_PLAY 1
#define BUTTON_RECORD 2
#define BUTTON_REWIND 3
#define BUTTON_WIDTH 30
#define BUTTON_HEIGHT 22


class NEWindow : public sf::RenderWindow
{
  public:
    NEWindow(sf::VideoMode mode, 
             const sf::String& title,
             const sf::ContextSettings& settings = sf::ContextSettings());
    virtual ~NEWindow();
    
    //NEWindow NEVER clean or deletes interfaces registred
    //Order Up to Down
    inline void registerInterface(Interface& i) {
      _interfaces.push_back(&i);
    }
    
    
    //NEWindow NEVER clean or deletes MouseCatcher registred
    inline void registerMouseCatcher(MouseCatcher& m) {
      _mouseCatchers.push_back(&m);
   
    }
    
    inline const sf::View getFullView() const {
      return _fullView;
    }
    
    inline const sf::Vector2f& clientSize() const {
      return _clientSize;
    }
    
    void setBackgroundTexture(const std::string& name,
                              const sf::Vector2i center);
    
    
    //Arrange interfaces disposal
    void arrange(); 
    bool useEvent(const sf::Event& event);
   
    bool checkInterrupt();
   
    void drawContent();
    
  protected:
    ///Paramètres de la fenêtre
    //Tailles des bordures
    float _borderSizeUp;
    float _borderSizeDown;
    float _borderSizeRight;
    float _borderSizeLeft;
    //Taille alloué pour les interfaces
    sf::Vector2f _clientSize; 
    sf::Vector2f _viewportMin;
    sf::Vector2f _viewportMax;
    
    //Variables de gestion de l'état de la fenêtre
    bool _onMoveWin;
    bool _onResizeWin;
    int _onClose;
    
    //previous positions...
    sf::Vector2i _previousWinPos;
    sf::Vector2i _previousMousePos;
    sf::Vector2u _previousWinSize;
    
    
#ifdef COMPILE_WINDOWS
    //Buttons
    Button _closeButton;
    
    //Resize Triangle
    sf::ConvexShape _resizeTriangle;
#endif    
    //Windows Title
    sf::Text _title;
    
    //Interface manager
    Interface* _currentInterfaceCatcher;
    std::vector<Interface*> _interfaces; 
    
    //Mouse Catcher Manager
    MouseCatcher* _currentMouseCatcher;
    std::vector<MouseCatcher*> _mouseCatchers;
    
    //windows FULL VIEW
    sf::View _fullView;
    
    //Background image
    sf::Sprite _backSprite;
    sf::Texture _backTexture;
    sf::Vector2i _backCenter;
    
};

class TrackControlBar : public Interface
{
  public:
    TrackControlBar(const sf::Vector2f &size);

    virtual ~TrackControlBar();
    
    inline Button& playButton() { return _playButton;}
    inline Button& recordButton() { return _recordButton;}
    
    void setTrack(Track* t);
    void setInstrument(AbstractInstrument* i);
  private:
    sf::Texture _buttonTexture;
    Button _playButton;
    Button _recordButton;
    SingleProcessButton _rewindButton;
    SingleProcessButton _loadMIDIButton;
    SingleProcessButton _saveMIDIButton;
    SingleProcessButton _loadPresetButton;
    SingleProcessButton _savePresetButton;
    
};

#endif
