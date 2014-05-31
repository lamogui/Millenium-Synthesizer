
/*******************************************************
Nom ......... : nelead6.hpp
Role ........ : Clone du Nord Lead 3
Auteur ...... : Julien DE LOOR 
Version ..... : V1.0 olol
Licence ..... : © Copydown™
********************************************************/

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
#define PARAM_NELEAD6_OSCMOD       0x01  //Modulation
#define PARAM_NELEAD6_FINETUNE     0x5E  //Effects 4 Depth (formerly Celeste Detune)
#define PARAM_NELEAD6_COARSETUNE   0x5D  //Effects 3 Depth (formerly Chorus Depth)
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
#define PARAM_NELEAD6_OSC1TYPE     0x50  //General-Purpose Controllers 5-8
#define PARAM_NELEAD6_OSC2TYPE     0x51  //General-Purpose Controllers 5-8
#define PARAM_NELEAD6_LFO1TYPE     0x52  //General-Purpose Controllers 5-8
#define PARAM_NELEAD6_LFO2TYPE     0x53  //General-Purpose Controllers 5-8
#define PARAM_NELEAD6_FILTERTYPE   0x41  //Portamento
#define PARAM_NELEAD6_MODTYPE      0x42  //Sostenuto
#define PARAM_NELEAD6_ENVFATTACK   0x10  //General-Purpose Controllers 1-4
#define PARAM_NELEAD6_ENVFDECAY    0x11  //General-Purpose Controllers 1-4
#define PARAM_NELEAD6_ENVFSUSTAIN  0x12  //General-Purpose Controllers 1-4
#define PARAM_NELEAD6_ENVFRELEASE  0x13  //General-Purpose Controllers 1-4
#define PARAM_NELEAD6_ENVFAMP      0x0B  //Expression Controller
#define PARAM_NELEAD6_LFO1DEST     0x63  //Non-Registered Parameter Number (MSB)

//modulation modes
#define NELEAD6_FM 1
#define NELEAD6_RM 2
#define NELEAD6_DIST 3

class NELead6Voice : public InstrumentVoice
{
  public:
    NELead6Voice(AbstractInstrument* creator);
    virtual ~NELead6Voice();

    void beginNote(Note& n);
    void endNote();
    void step(Signal* leftout, Signal* rightout=0);
    
    inline void replaceOsc1(Oscillator* o) { delete _osc1; _osc1=o;}
    inline void replaceOsc2(Oscillator* o) { delete _osc2; _osc2=o;}
    inline void replaceLfo1(Oscillator* o) { delete _lfo1; _lfo1=o;}
    inline void replaceLfo2(Oscillator* o) { delete _lfo2; _lfo2=o;}
    inline void replaceFilter(Filter* f) { delete _filter1; _filter1=f;}
    
  private:
    Oscillator* _osc1;
    Oscillator* _osc2;
    Oscillator* _lfo1;
    Oscillator* _lfo2;
    Filter* _filter1;
    Enveloppe _env;
    Enveloppe _filterEnv;
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
    
    void notify(InstrumentParameter* p);
    
  protected :
    InstrumentParameter _oscmix;
    InstrumentParameter _oscmod;
    InstrumentParameter _finetune;
    InstrumentParameter _coarsetune;
    InstrumentParameter _lfo1_amount;
    InstrumentParameter _lfo1_rate;
    InstrumentParameter _lfo2_amount;
    InstrumentParameter _lfo2_rate;
    InstrumentParameter _env_attack;
    InstrumentParameter _env_decay;
    InstrumentParameter _env_sustain;
    InstrumentParameter _env_release;
    InstrumentParameter _envf_attack;
    InstrumentParameter _envf_decay;
    InstrumentParameter _envf_sustain;
    InstrumentParameter _envf_release;
    InstrumentParameter _envf_amount;
    InstrumentParameter _osc1_shape;
    InstrumentParameter _osc2_shape;
    InstrumentParameter _filter1_rate;
    InstrumentParameter _filter1_resonance;
    InstrumentParameter _osc1_type;
    InstrumentParameter _osc2_type;
    InstrumentParameter _lfo1_type;
    InstrumentParameter _lfo2_type;
    InstrumentParameter _filter_type;
    InstrumentParameter _modulation_type;
	InstrumentParameter _lfo1_dest;
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

class NELead6TriangleLED : public MouseCatcher, public sf::Transformable
{
  public:
    NELead6TriangleLED(InstrumentParameter* p, const sf::Texture &texture, const sf::IntRect &rect);
    virtual ~NELead6TriangleLED();
    
    
    virtual void update();
    virtual bool onMousePress(float x, float y);
    virtual void onMouseMove(float x, float y);
    virtual void onMouseRelease(float x, float y);
    void draw (sf::RenderTarget &target, sf::RenderStates states) const;
    
  private:
    InstrumentParameter* _param;
    sf::Sprite _sprite;
    sf::ConvexShape _light;
};


class NELead6Interface : public Interface
{
  public:
    NELead6Interface(NELead6* instrument, const sf::Vector2f& size);
    virtual ~NELead6Interface();

    virtual inline sf::Color getColor() //Couleur de l'interface
    {
      return sf::Color(255,0,0,0);
    }

  protected:
    sf::Texture _texture;
    sf::Sprite _back;
    NELead6* _instrument;
    NELead6TriangleLED* _osc1TypeLED;
    NELead6TriangleLED* _osc2TypeLED;
    NELead6TriangleLED* _lfo1TypeLED;
    NELead6TriangleLED* _lfo2TypeLED;
    NELead6TriangleLED* _filterTypeLED;
    NELead6TriangleLED* _modTypeLED;
    NELead6TriangleLED* _lfo1DestLED;
    NELead6Knob* _outputKnob;
    NELead6Knob* _oscmixKnob;
    NELead6Knob* _oscmodKnob;
    NELead6Knob* _finetuneKnob;
    NELead6Knob* _coarsetuneKnob;
    NELead6Knob* _lfo1AmKnob;
    NELead6Knob* _lfo1RateKnob;
    NELead6Knob* _lfo2AmKnob;
    NELead6Knob* _lfo2RateKnob;
    NELead6Knob* _envAttackKnob;
    NELead6Knob* _envDecayKnob;
    NELead6Knob* _envSustainKnob;
    NELead6Knob* _envReleaseKnob;
    NELead6Knob* _envFilterAttackKnob;
    NELead6Knob* _envFilterDecayKnob;
    NELead6Knob* _envFilterSustainKnob;
    NELead6Knob* _envFilterReleaseKnob;
    NELead6Knob* _envFilterAmountKnob;
    NELead6Knob* _osc1ShapeKnob;
    NELead6Knob* _osc2ShapeKnob;
    NELead6Knob* _filter1RateKnob;
    NELead6Knob* _filter1ResKnob;
    InstrumentButton* _osc1TypeButton;
    InstrumentButton* _osc2TypeButton;
    InstrumentButton* _lfo1TypeButton;
    InstrumentButton* _lfo2TypeButton;
    InstrumentButton* _filterTypeButton;
    InstrumentButton* _modTypeButton;
    InstrumentButton* _lfo1DestButton;
};



#endif
