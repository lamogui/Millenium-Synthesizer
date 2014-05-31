
#include "menuBar.hpp"

MenuBar::MenuBar(const sf::Vector2i& zone, const sf::Vector2f &size, int id, int state, std::string texture_url) :
  Interface(zone, size),
  _buttonMenu(std::map())
{
  _buttonTexture.loadFromFile(texture_url);
}

MenuBar::MenuBar(const sf::Vector2i& zone, const sf::Vector2f &size,
    std::map<int, Button*> &menu) :
  Interface(zone, size),
  _buttonMenu(menu)
{
}

Button* MenuBar::get_button(int id) {
  return _buttonMenu[id];
}

void MenuBar::add_button(int id, Button* button){
  _buttonMenu[id]=button;
}

MenuBar::~MenuBar() {
}

