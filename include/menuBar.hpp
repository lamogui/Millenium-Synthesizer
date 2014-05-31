#ifndef MENUBAR_HPP
#define MENUBAR_HPP

#include "interface.hpp"
#include <map>

class MenuBar : public Interface
{
  public:
    MenuBar(const sf::Vector2i& zone, const sf::Vector2f &size, int id, int state, std::string texture_url);
    MenuBar(const sf::Vector2i& zone, const sf::Vector2f &size, 
                                      std::map<int, Button*> &menu);
    virtual ~MenuBar();

    Button* get_button(int id);
    void add_button(int id, int state);
  private:
    sf::Texture _buttonTexture;
    sf::Vector2i _xy_buttonTexture;
    sf::Vector2i _xy_menuBar;
    std::map<int,Button*> _buttonMenu;
};

#define PLAY 1
#define RECORD 2
#define REWIND 3

#define BUTTON_WIDTH 22
#define BUTTON_HEIGHT 30

#endif
