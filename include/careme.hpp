/*******************************************************
Nom ......... : careme.hpp
Role ........ : Instrument de Mr CHOI
Auteur ...... : Kwon-Young CHOI
Version ..... : V1.7 olol
Licence ..... : © Copydown™
********************************************************/

#ifndef __CAREME
#define __CAREME

#include "instrument.hpp"
#include "oscillator.hpp"
#include "interface.hpp"
#include "enveloppe.hpp"

#include "nelead6.hpp"

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
    
    //if rightout then user want a mono signal
    void step(Signal* leftout, Signal* rightout=0);
    
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
    NELead6Knob* _envAttackKnob;
    NELead6Knob* _envDecayKnob;
    NELead6Knob* _envSustainKnob;
    NELead6Knob* _envReleaseKnob;
};

#endif

