
#ifndef __NELEAD6
#define __NELEAD6

#include "instrument.hpp"
#include "oscillator.hpp"
#include "interface.hpp"
#include "enveloppe.hpp"

//Param defines

#define PARAM_NELEAD6_OSCMIX 0
#define PARAM_NELEAD6_LFO1RATE 1
#define PARAM_NELEAD6_LFO1AMOUNT 2
#define PARAM_NELEAD6_LFO2RATE 3
#define PARAM_NELEAD6_LFO2AMOUNT 4
#define PARAM_NELEAD6_ENVATTACK 5
#define PARAM_NELEAD6_ENVDECAY 6
#define PARAM_NELEAD6_ENVSUSTAIN 7
#define PARAM_NELEAD6_ENVRELEASE 8
#define PARAM_NELEAD6_OSC1SHAPE 9
#define PARAM_NELEAD6_OSC2SHAPE 10

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
    sf::RectangleShape _movingSelector;
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
};



#endif
