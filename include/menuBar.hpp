#ifndef MENUBAR_HPP
#define MENUBAR_HPP

#include "interface.hpp"
#include <map>

class MenuBar : public Interface
{
  public:
    MenuBar(const sf::Vector2i& zone, 
            const sf::Vector2f &size, 
            std::string texture_url);
    virtual ~MenuBar();

  private:
    sf::Texture _buttonTexture;
    sf::vector2i _xy_buttonTexture;
    sf::Vector2i _xy_menuBar;
};

#define PLAY 1
#define RECORD 2
#define REWIND 3

#define BUTTON_WIDTH 22
#define BUTTON_HEIGHT 30

#endif
