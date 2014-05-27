#ifndef MENUBAR_HPP
#define MENUBAR_HPP

#include "interface.hpp"
#include <map>

class MenuBar : public Interface
{
  public:
    MenuBar(const sf::Vector2i& zone, const sf::Vector2f &size, int id, Button *button);
    MenuBar(const sf::Vector2i& zone, const sf::Vector2f &size, 
                                      std::map<int, Button*> &menu);
    virtual ~MenuBar();

    Button* get_button(int id);
    void add_button(int id, Button* button);
  private:
    std::map<int,Button*> _buttonMenu;
};

#endif
