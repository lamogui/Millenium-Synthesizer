#ifndef __CAREME
#define __CAREME

#include "instrument.hpp"
#include "oscillator.hpp"
#include "interface.hpp"
#include "enveloppe.hpp"

#define PARAM_CAREME_ENVATTACK 1
#define PARAM_CAREME_ENVDECAY 2
#define PARAM_CAREME_ENVSUSTAIN 3
#define PARAM_CAREME_ENVRELEASE 4
/*
#define PARAM_CAREME_LFOAMP_ID 1
#define PARAM_CAREME_LFOFREQUENCY_ID 2
#define PARAM_CAREME_DUTY_ID 3
*/
class CaremeVoice : public InstrumentVoice
{
  public:
    //Instruments Voice should only be created by instruments
    CaremeVoice(AbstractInstrument* creator);
    virtual ~CaremeVoice();
    
    virtual void beginNote(Note& n);
    virtual void endNote();
    
    virtual void step(Signal* output);
    
  protected:
    SinusoidalOscillator* _osc1;
    SinusoidalOscillator* _osc2;
    SinusoidalOscillator* _osc3;
    Enveloppe _env;
};

class Careme : public Instrument<CaremeVoice>
{
  public:
    Careme();
    virtual ~Careme();
  
    virtual InstrumentParameter* getParameter(unsigned char id);
    virtual void step(Signal* output);
  
  protected:
    InstrumentParameter _env_attack;
    InstrumentParameter _env_decay;
    InstrumentParameter _env_sustain;
    InstrumentParameter _env_release;
    /*
    InstrumentParameter _lfo_amp;
    InstrumentParameter _lfo_frequency;
    InstrumentParameter _duty;
    */
};
class CaremeKnob : public Knob
{
  public:
    CaremeKnob(InstrumentParameter* p, const sf::Texture &texture, const sf::IntRect &backRect, const sf::IntRect &knobRect);
    virtual ~CaremeKnob();
    
    void update();
    void draw (sf::RenderTarget &target, sf::RenderStates states) const;
  
  private:
    sf::RectangleShape _selector;
    sf::RectangleShape _movingSelector;
};

class CaremeInterface : public Interface
{
  public:
    CaremeInterface(Careme* instrument, const sf::Vector2f& size);
    virtual ~CaremeInterface();
    
  protected:
    sf::Texture _texture;
    sf::Sprite _back;
    Careme* _instrument;
    /*
    CaremeKnob* _lfoAmpKnob;
    CaremeKnob* _lfoFrequencyKnob;
    CaremeKnob* _dutyKnob; 
    */
    CaremeKnob* _envAttackKnob;
    CaremeKnob* _envDecayKnob;
    CaremeKnob* _envSustainKnob;
    CaremeKnob* _envReleaseKnob;
};

#endif

