#ifndef __PURESQUARE
#define __PURESQUARE

#include "instrument.hpp"
#include "oscillator.hpp"
#include "interface.hpp"

#define PARAM_PURESQUARE_LFOAMP_ID 1
#define PARAM_PURESQUARE_LFOFREQUENCY_ID 2
#define PARAM_PURESQUARE_DUTY_ID 3

class PureSquareVoice : public InstrumentVoice
{
  public:
    //Instruments Voice should only be created by instruments
    PureSquareVoice(AbstractInstrument* creator);
    virtual ~PureSquareVoice();
    
    virtual void beginNote(Note& n);
    virtual void endNote();
    
    virtual void step(Signal* output);
    
  protected:
    SquareOscillator _osc;
    SinusoidalOscillator _lfo;
};

class PureSquare : public Instrument<PureSquareVoice>
{
  public:
    PureSquare();
    virtual ~PureSquare();
  
  virtual InstrumentParameter* getParameter(unsigned char id);
  
  protected:
    InstrumentParameter _lfo_amp;
    InstrumentParameter _lfo_frequency;
    InstrumentParameter _duty;
};

class PureSquareInterface : public Interface
{
  public:
    PureSquareInterface(PureSquare* instrument, const sf::Vector2f& size);
    virtual ~PureSquareInterface();
    
  protected:
    sf::Texture _texture;
    sf::Sprite _back;
    Knob* _lfoAmpKnob;
    Knob* _lfoFrequencyKnob;
    Knob* _dutyKnob; 
    PureSquare* _instrument;
};

#endif

