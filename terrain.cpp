
#include "terrain.hpp"

BasicTerrain::BasicTerrain() : 
Terrain(),
_grassColor(45,144,0),
_linesColor(255,255,255),
_grass(sf::Vector2f(width,height)),
_middleCircle(9.15,42),
_middleLine(sf::Vector2f(width,0.12)),
_upSurface(sf::Vector2f(40.3,16.5)),
_downSurface(sf::Vector2f(40.3,16.5)),
_upGoalSurface(sf::Vector2f(18.3,5.5)),
_downGoalSurface(sf::Vector2f(18.3,5.5)),
_upCircle(9.15,42),
_downCircle(9.15,42)
{
  generate();
}

void BasicTerrain::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
  target.draw(_grass);
  target.draw(_upCircle);
  target.draw(_upSurface);
  target.draw(_upGoalSurface);
  target.draw(_middleCircle);
  target.draw(_middleLine);
  target.draw(_downCircle);
  target.draw(_downSurface);
  target.draw(_downGoalSurface);
}

void BasicTerrain::generate()
{
  _grass.setSize(sf::Vector2f(width,height));
  _middleLine.setSize(sf::Vector2f(width,0.12));
  
  _grass.setFillColor(_grassColor);
  _grass.setOrigin (width/2.0 , height/2.0);
  _grass.setOutlineThickness(0.12);
  _grass.setOutlineColor(_linesColor);
  _middleCircle.setFillColor(_grassColor);
  _middleCircle.setOutlineThickness(0.12);
  _middleCircle.setOutlineColor(_linesColor);
  _middleCircle.setOrigin (9.15 , 9.15);
  _middleLine.setFillColor(_linesColor);
  _middleLine.setOrigin (width/2.0 , 0.06);
  
  _upSurface.setOrigin(40.3*0.5,0.0);
  _downSurface.setOrigin(40.3*0.5,16.5);
  
  _upSurface.setPosition(0.0,-height*0.5);
  _downSurface.setPosition(0.0,height*0.5);
  
  _upSurface.setOutlineThickness(0.12);
  _downSurface.setOutlineThickness(0.12);
  
  _upSurface.setOutlineColor(_linesColor);
  _downSurface.setOutlineColor(_linesColor);
  
  _upSurface.setFillColor(_grassColor);
  _downSurface.setFillColor(_grassColor);
  
  _upGoalSurface.setOrigin(18.3*0.5,0.0);
  _downGoalSurface.setOrigin(18.3*0.5,5.5);
  
  _upGoalSurface.setPosition(0.0,-height*0.5);
  _downGoalSurface.setPosition(0.0,height*0.5);
  
  _upGoalSurface.setOutlineThickness(0.12);
  _downGoalSurface.setOutlineThickness(0.12);
  
  _upGoalSurface.setOutlineColor(_linesColor);
  _downGoalSurface.setOutlineColor(_linesColor);
  
  _upGoalSurface.setFillColor(_grassColor);
  _downGoalSurface.setFillColor(_grassColor);
  
  
  _upCircle.setOrigin (9.15 , 9.15);
  _downCircle.setOrigin (9.15 , 9.15);
  
  _upCircle.setPosition(0.0,-height*0.5 + 11.0);
  _downCircle.setPosition(0.0,height*0.5 - 11.0);
  
  _upCircle.setOutlineThickness(0.12);
  _downCircle.setOutlineThickness(0.12);
  
  _upCircle.setOutlineColor(_linesColor);
  _downCircle.setOutlineColor(_linesColor);
  
  _upCircle.setFillColor(_grassColor);
  _downCircle.setFillColor(_grassColor);
  
  
}