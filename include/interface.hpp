#ifndef __INTERFACE
#define __INTERFACE

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "note.hpp"

class MouseCatcher : public sf::Drawable
{
  public:
    //Coordinates must be view converted !
    virtual bool onMousePress(float x, float y)=0; //return true if the device catch the mouse
    virtual void onMouseMove(float x, float y)=0;
    virtual void onMouseRelease(float x, float y)=0;
    //update something ?
    virtual void update()=0;
};

class Knob : public MouseCatcher, public sf::Transformable
{
  public:
    Knob();
    Knob(InstrumentParameter* p, const sf::Texture &texture, const sf::IntRect &backRect, const sf::IntRect &knobRect);
    virtual ~Knob();
    
    
    inline void setParam(InstrumentParameter* p) {_param=p;}
    void setBackTexture(const sf::Texture &texture,const sf::IntRect &rect);
    void setKnobTexture(const sf::Texture &texture,const sf::IntRect &rect);
    
    virtual bool onMousePress(float x, float y);
    virtual void onMouseMove(float x, float y);
    virtual void onMouseRelease(float x, float y);
    virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const;
    virtual void update();
    
    sf::Color overColor;
    
  protected:
    sf::Sprite _back_sprite;
    sf::Sprite _knob_sprite;
    InstrumentParameter* _param;
    bool _catched;
    float _catch_x;
    float _catch_y;
    float _catch_angle;
};

class ScrollBar : public MouseCatcher
{
  public:
    ScrollBar(sf::View& view, int zone,bool horizontal=false);
    virtual ~ScrollBar();
    
    virtual bool onMousePress(float x, float y);
    virtual void onMouseMove(float x, float y);
    virtual void onMouseRelease(float x, float y);
    virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const;
    virtual void update();

    
  private:
    sf::View *_view;
    sf::RectangleShape _bar;
    sf::RectangleShape _decoration;
    float _catch;
    int _current_offset;
    int _zone_size;
    bool _horizontal;
};
/*
class Fader : public MouseCatcher {
  public:
    Fader(InstrumentParameter* p, const sf::Texture &texture, const sf::IntRect &backRect, const sf::IntRect &faderRect);
    virtual ~Fader();
    
    virtual bool onMousePress(float x, float y);
    virtual void onMouseMove(float x, float y);
    virtual void onMouseRelease(float x, float y);
    virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const;
    
  protected:
    sf::Sprite _back_sprite;
    sf::Sprite _fader_sprite;
    InstrumentParameter* _param;
    float _catch_x;
    float _catch_y;
    float _catch_translation;
};
*/
class Interface : public sf::Drawable
{
  public:
    Interface(const sf::Vector2i& zone,const sf::Vector2f &size);
    virtual ~Interface();

    //is the interface have an object to interact with
    //x and y in opengl coordinates
    MouseCatcher* onMousePress(float x, float y);


    inline void setViewport(const sf::FloatRect &viewport)
    {
      _view.setViewport(viewport);
    }
    inline sf::View& getView() 
    {
      return _view;
    }

    void setViewSize(float x, float y);
    void draw(sf::RenderTarget &target, sf::RenderStates states) const;
    void update();

    void addMouseCatcher(MouseCatcher* c);
    void addDrawable(sf::Drawable* c);

  protected:
    std::vector<MouseCatcher*> _mouseCatcher;
    std::vector<sf::Drawable*> _drawables;
    sf::View _view;
    sf::Vector2i _zone;
    ScrollBar _verticalBar;
    ScrollBar _horizontalBar;
};



#endif
