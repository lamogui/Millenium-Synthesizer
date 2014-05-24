/*******************************************************
Nom ......... : interface.hpp
Role ........ : Classes de base pour l'interface 
                compatible avec opengl
Auteur ...... : Julien DE LOOR 
Version ..... : V1.0 olol
Licence ..... : © Copydown™
********************************************************/

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
    virtual void onMouseOver(float x, float y) {
      (void) x; (void) y;
    }
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
    inline sf::Color overColor(){return _overColor;}
    inline void setOverColor(const sf::Color& c){_overColor = c;}
    
    virtual bool onMousePress(float x, float y);
    virtual void onMouseMove(float x, float y);
    virtual void onMouseRelease(float x, float y);
    virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const;
    virtual void update();
	
    
    
    
  protected:
    sf::Color _overColor;
    sf::Sprite _back_sprite;
    sf::Sprite _knob_sprite;
    InstrumentParameter* _param;
    bool _catched;
    float _catch_x;
    float _catch_y;
    float _catch_angle;
};

namespace ButtonMode {
  typedef enum {
    toggle,
    increment,
    decrement,
    on,
    off,
    interrupt
  } Mode;
}

class Button : public MouseCatcher, public sf::Transformable
{
  public:
    Button(const sf::Vector2f& size, const sf::String text);
    Button(InstrumentParameter* p, const sf::Texture &texture
                                 , const sf::IntRect &idle
                                 , const sf::IntRect &clicked
                                 , ButtonMode::Mode mode=ButtonMode::toggle);
    Button(int *val, const sf::Texture &texture
                   , const sf::IntRect &idle
                   , const sf::IntRect &clicked
                   , ButtonMode::Mode mode=ButtonMode::toggle);
    virtual ~Button();
    
    inline void setParam(InstrumentParameter* p,
                         ButtonMode::Mode mode=ButtonMode::toggle) {
                            _param=p;
                            _mode=mode;
                            if (mode==ButtonMode::toggle && _param)
                            {
                              if (_param->active()) _shape.setTextureRect(_clickedRect);
                              else _shape.setTextureRect(_idleRect);
                            }
                         }
    
    //Val setter
    inline void linkTo(int* v) { 
      _val=v;
      if (_mode==ButtonMode::toggle && _val)
       {
         if (*_val) _shape.setTextureRect(_clickedRect);
         else _shape.setTextureRect(_idleRect);
       }
    }
    
    inline void forceOn()
    {
      if (_mode==ButtonMode::toggle)
      {
         if (_val) *_val=1;
         if (_param) _param->on();
         _shape.setTextureRect(_clickedRect);
      }
    }
    
    inline void forceOff()
    {
      if (_mode==ButtonMode::toggle)
      {
         if (_val) *_val=0;
         if (_param) _param->off();
         _shape.setTextureRect(_idleRect);
      }
    }
    
    //Shape setters    
    inline void setOutlineThickness(float f) {_shape.setOutlineThickness(f);}
    inline void setOutlineColor(const sf::Color& c) {_shape.setOutlineColor(c); _text.setColor(c);}

    //Color setters
    inline void setColors(const sf::Color& i, const sf::Color& c) {setClickedColor(c);setIdleColor(i);}
    inline void setClickedColor(const sf::Color& c) {_clickedColor=c;}
    inline void setIdleColor(const sf::Color& i) {_idleColor=i; _shape.setFillColor(_idleColor);}
    
    //Text setter
    void setText(const sf::String& t);
    inline sf::Text& getText() { return _text; }
    
    //Texture setter         
    void setTexture(const sf::Texture &texture, const sf::IntRect &idle, const sf::IntRect &clicked);
    
    virtual bool onMousePress(float x, float y);
    virtual void onMouseMove(float x, float y);
    virtual void onMouseRelease(float x, float y);
    virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const;
    virtual void update();
    
    
  protected:
    sf::Color _idleColor;
    sf::Color _clickedColor;
    sf::RectangleShape _shape;
    sf::Text _text;
    sf::IntRect _idleRect;
    sf::IntRect _clickedRect;
    InstrumentParameter* _param;
    int* _val; 
    bool _catched;
    ButtonMode::Mode _mode;
};

class ScrollBar : public MouseCatcher
{
  public:
    ScrollBar(sf::View& view, unsigned int zone,bool horizontal=false);
    virtual ~ScrollBar();
    
    virtual bool onMousePress(float x, float y);
    virtual void onMouseMove(float x, float y);
    virtual void onMouseRelease(float x, float y);
    virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const;
    virtual void update();

    void setZoneSize(unsigned int zone);
    
    
  private:
    sf::View *_view;
    sf::RectangleShape _bar;
    sf::RectangleShape _decoration;
    float _catch;
    int _current_offset;
    unsigned int _zone_size;
    bool _horizontal;
};

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

class Interface : public sf::Drawable
{
  public:
    Interface(const sf::Vector2i& zone,const sf::Vector2f &size);
    virtual ~Interface();

    //is the interface have an object to interact with
    //x and y in opengl coordinates
    MouseCatcher* onMousePress(float x, float y);

    
    //is the mouse on the interface now ?
    bool onIt(unsigned int x, unsigned int y, 
              unsigned int sx, unsigned int sy);

    inline void setViewport(const sf::FloatRect &viewport)
    {
      _view.setViewport(viewport);
    }
    inline sf::View& getView() 
    {
      return _view;
    }
    
    virtual inline sf::Vector2i getIdealSize() //pas forcement la "zone occupé"
    {
      return _zone;
    }
    
    virtual inline sf::Color getColor() //Couleur de l'interface
    {
      return sf::Color(255,255,255,0);
    }

    virtual void setViewSize(float x, float y);
    void draw(sf::RenderTarget &target, sf::RenderStates states) const;
    virtual void update();

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
