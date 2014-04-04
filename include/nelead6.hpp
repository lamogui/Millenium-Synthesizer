
#ifndef __NELEAD6
#define __NELEAD6

#include "instrument.hpp"
#include "oscillator.hpp"
#include "interface.hpp"
#include "enveloppe.hpp"
#include "filter.hpp"

//Param defines
//        PARAM NAME               MIDI ID   MIDI NAME
#define PARAM_NELEAD6_OSCMIX       0x08  //Balance
#define PARAM_NELEAD6_LFO1RATE     0x20  //LSB for controllers 0-31
#define PARAM_NELEAD6_LFO1AMOUNT   0x0C  //Effect Control 1
#define PARAM_NELEAD6_LFO2RATE     0x21  //LSB for controllers 0-31
#define PARAM_NELEAD6_LFO2AMOUNT   0x0D  //Effect Control 2
#define PARAM_NELEAD6_ENVATTACK    0x49  //Sound Controller 4 (default: Attack Time)
#define PARAM_NELEAD6_ENVDECAY     0x4A  //Sound Controller 6
#define PARAM_NELEAD6_ENVSUSTAIN   0x4B  //Sound Controller 7
#define PARAM_NELEAD6_ENVRELEASE   0x48  //Sound Controller 3 (default: Release Time)
#define PARAM_NELEAD6_OSC1SHAPE    0x46  //Sound Controller 1 (default: Timber Variation)
#define PARAM_NELEAD6_OSC2SHAPE    0x47  //Sound Controller 2 (default: Timber/Harmonic Content)
#define PARAM_NELEAD6_FILTER1RATE  0x4C  //Sound Controller 8
#define PARAM_NELEAD6_FILTER1RES   0x4D  //Sound Controller 9

//modulation modes
#define NELEAD6_FM 0
#define NELEAD6_RM 1

class NELead6Voice : public InstrumentVoice
{
  public:
    NELead6Voice(AbstractInstrument* creator);
    virtual ~NELead6Voice();

    void beginNote(Note& n);
    void endNote();
    void step(Signal* leftout, Signal* rightout=0);
    
  private:
    Oscillator* _osc1;
    Oscillator* _osc2;
    Oscillator* _lfo1;
    Oscillator* _lfo2;
    Filter* _filter1;
    Enveloppe _env;
    Note _currentNote;
    Signal _oscmix;
};


class NELead6 : public Instrument<NELead6Voice>
{
  public: 
    NELead6();
    virtual ~NELead6();
    
    virtual InstrumentParameter* getParameter(unsigned char id);
    void step(Signal* leftout, Signal* rightout=0);
    
  protected :
    InstrumentParameter _oscmix;
    InstrumentParameter _lfo1_amount;
    InstrumentParameter _lfo1_rate;
    InstrumentParameter _lfo2_amount;
    InstrumentParameter _lfo2_rate;
    InstrumentParameter _env_attack;
    InstrumentParameter _env_decay;
    InstrumentParameter _env_sustain;
    InstrumentParameter _env_release;
    InstrumentParameter _osc1_shape;
    InstrumentParameter _osc2_shape;
    InstrumentParameter _filter1_rate;
    InstrumentParameter _filter1_resonance;
}; 

class NELead6Knob : public Knob
{
  public:
    NELead6Knob(InstrumentParameter* p, const sf::Texture &texture, const sf::IntRect &backRect, const sf::IntRect &knobRect);
    virtual ~NELead6Knob();
    
    void update();
    void draw (sf::RenderTarget &target, sf::RenderStates states) const;
  
  private:
    sf::RectangleShape _selector;
    sf::RectangleShape _autoSelector;
};

class NELead6Interface : public Interface
{
  public:
    NELead6Interface(NELead6* instrument, const sf::Vector2f& size);
    virtual ~NELead6Interface();
    
  protected:
    sf::Texture _texture;
    sf::Sprite _back;
    NELead6* _instrument;
    NELead6Knob* _outputKnob;
    NELead6Knob* _oscmixKnob;
    NELead6Knob* _lfo1AmKnob;
    NELead6Knob* _lfo1RateKnob;
    NELead6Knob* _lfo2AmKnob;
    NELead6Knob* _lfo2RateKnob;
    NELead6Knob* _envAttackKnob;
    NELead6Knob* _envDecayKnob;
    NELead6Knob* _envSustainKnob;
    NELead6Knob* _envReleaseKnob;
    NELead6Knob* _osc1ShapeKnob;
    NELead6Knob* _osc2ShapeKnob;
    NELead6Knob* _filter1RateKnob;
    NELead6Knob* _filter1ResKnob;
};



#endif
