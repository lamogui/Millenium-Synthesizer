#ifndef __INTERFACE
#define __INTERFACE

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "note.hpp"

class MouseCatcher : public sf::Drawable, public sf::Transformable
{
  public:
    //Coordinates must be view converted !
    virtual bool onMousePress(float x, float y)=0; //return true if the device catch the mouse
    virtual void onMouseMove(float x, float y)=0;
    virtual void onMouseRelease(float x, float y)=0;
    //update something ?
    virtual void update()=0;
};

class Knob : public MouseCatcher
{
  public:
    Knob(InstrumentParameter* p, const sf::Texture &texture, const sf::IntRect &backRect, const sf::IntRect &knobRect);
    virtual ~Knob();
    
    virtual bool onMousePress(float x, float y);
    virtual void onMouseMove(float x, float y);
    virtual void onMouseRelease(float x, float y);
    virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const;
    virtual void update();
  private:
    sf::Sprite _back_sprite;
    sf::Sprite _knob_sprite;
    InstrumentParameter* _param;
    float _catch_x;
    float _catch_y;
    float _catch_angle;
    
};



#endif