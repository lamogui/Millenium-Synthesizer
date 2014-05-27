

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
    
};

#endif
