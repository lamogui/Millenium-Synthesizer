#ifndef __CAREME
#define __CAREME

#include "instrument.hpp"
#include "oscillator.hpp"
#include "interface.hpp"
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
    SinusoidalOscillator _osc1;
    SinusoidalOscillator _osc2;
    SinusoidalOscillator _osc3;
};
/*
class Careme : public Instrument<CaremeVoice>
{
  public:
    Careme();
    virtual ~Careme();
  
  virtual InstrumentParameter* getParameter(unsigned char id);
  
  protected:
    InstrumentParameter _lfo_amp;
    InstrumentParameter _lfo_frequency;
    InstrumentParameter _duty;
};

class CaremeInterface : public Interface
{
  public:
    CaremeInterface(Careme* instrument, const sf::Vector2f& size);
    virtual ~CaremeInterface();
    
  protected:
    sf::Texture _texture;
    sf::Sprite _back;
    Knob* _lfoAmpKnob;
    Knob* _lfoFrequencyKnob;
    Knob* _dutyKnob; 
    Careme* _instrument;
};
*/
#endif

