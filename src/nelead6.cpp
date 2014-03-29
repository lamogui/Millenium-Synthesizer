#include "nelead6.hpp"
#include <cmath>

NELead6Voice::NELead6Voice(AbstractInstrument* creator) :
InstrumentVoice(creator),
_osc1(new SawOscillator),
_osc2(new TriangleOscillator),
_lfo1(new SawOscillator),
_lfo2(new TriangleOscillator),
_currentNote(0,NOT_A_NOTE)
{
  
}

NELead6Voice::~NELead6Voice()
{
  delete _osc1;
  delete _osc2;
  delete _lfo1;
  delete _lfo2;
}

void NELead6Voice::beginNote(Note& n)
{
  _currentNote = n;
  _osc1->resetTime();
  _osc2->resetTime();
  _lfo1->resetTime();
  _lfo2->resetTime();
  _env.resetTime();
  
  _osc1->setFrequency(_currentNote.frequency());
  _osc1->setAmplitude(_currentNote.velocity);
  _osc2->setFrequency(_currentNote.frequency());
  _osc2->setAmplitude(_currentNote.velocity);

  _used=true;
}

void NELead6Voice::endNote()
{
  _env.beginRelease();
}

void NELead6Voice::step(Signal* leftout, Signal* rightout)
{

  float oscmix = _instrument->getParameter(PARAM_NELEAD6_OSCMIX)->getValue()/255.f;
  
  float lfo1_rate = exp(_instrument->getParameter(PARAM_NELEAD6_LFO1RATE)->getValue()*9.70f/255.f - 3.50f);
  float lfo2_rate = exp(_instrument->getParameter(PARAM_NELEAD6_LFO2RATE)->getValue()*9.70f/255.f - 3.50f);
  
  float lfo1_amount = _instrument->getParameter(PARAM_NELEAD6_LFO1AMOUNT)->getValue()/255.f;
  float lfo2_amount = _instrument->getParameter(PARAM_NELEAD6_LFO2AMOUNT)->getValue()/255.f;
  
  float osc1_shape = _instrument->getParameter(PARAM_NELEAD6_OSC1SHAPE)->getValue()/255.f;
  float osc2_shape = _instrument->getParameter(PARAM_NELEAD6_OSC2SHAPE)->getValue()/255.f;
  
  _env.attack = _instrument->getParameter(PARAM_NELEAD6_ENVATTACK)->getValue()*Signal::frequency/80;
  _env.decay = _instrument->getParameter(PARAM_NELEAD6_ENVDECAY)->getValue()*Signal::frequency/80;
  _env.sustain = (float)_instrument->getParameter(PARAM_NELEAD6_ENVSUSTAIN)->getValue()/255.f;
  _env.release = _instrument->getParameter(PARAM_NELEAD6_ENVRELEASE)->getValue()*Signal::frequency/80;
  
  _lfo1->setAmplitude(lfo1_amount);
  _lfo2->setAmplitude(lfo2_amount);
  
  _lfo1->setFrequency(lfo1_rate*_currentNote.frequency()/440.f);
  _lfo2->setFrequency(lfo2_rate*_currentNote.frequency()/440.f);
  
  Signal* lfo1 = _lfo1->generate();
  Signal* lfo2 = _lfo2->generate(); 
  
  lfo2->scale(0.5f);
  
  _osc1->setShape(osc1_shape);
  _osc2->setShape(osc2_shape);
  _osc2->getShape().add(lfo2);
  _osc2->getShape().saturate(0.f,1.f);
  
  //final mixing
  
  _oscmix.constant(oscmix);
  lfo1->scale(0.5f);
  _oscmix.add(lfo1);
  _oscmix.saturate(0.f,1.f);
  
  _osc1->step(leftout);
  Signal* osc2 = _osc2->generate();

  osc2->mix(&_oscmix);
  
  _oscmix.scale(-1.f);
  _oscmix.addOffset(1.f);
  
  leftout->mix(&_oscmix);
  
  leftout->add(osc2);
  
  leftout->mix(_env.generate());
  if (rightout)
    *rightout = *leftout;
  
  if (_env.hasEnded()) _used = false;
}

NELead6::NELead6() :
Instrument<NELead6Voice>(),
_oscmix(0,0,255),
_lfo1_amount(0,0,255),
_lfo1_rate(0,0,255),
_lfo2_amount(0,0,255),
_lfo2_rate(0,0,255),
_env_attack(100,0,255),
_env_decay(50,0,255),
_env_sustain(200,0,255),
_env_release(80,0,255),
_osc1_shape(0,0,255),
_osc2_shape(0,0,255)
{
  
}
 
NELead6::~NELead6()
{
}

InstrumentParameter* NELead6::getParameter(unsigned char id)
{
  switch (id)
  {
    case PARAM_NELEAD6_OSCMIX: return &_oscmix;
    case PARAM_NELEAD6_LFO1RATE: return &_lfo1_rate;
    case PARAM_NELEAD6_LFO1AMOUNT: return &_lfo1_amount;
    case PARAM_NELEAD6_LFO2RATE: return &_lfo2_rate;
    case PARAM_NELEAD6_LFO2AMOUNT: return &_lfo2_amount; 
    case PARAM_NELEAD6_ENVATTACK: return &_env_attack;
    case PARAM_NELEAD6_ENVDECAY: return &_env_decay;
    case PARAM_NELEAD6_ENVSUSTAIN: return &_env_sustain;
    case PARAM_NELEAD6_ENVRELEASE: return &_env_release;
    case PARAM_NELEAD6_OSC1SHAPE: return &_osc1_shape;
    case PARAM_NELEAD6_OSC2SHAPE: return &_osc2_shape;
    default : return Instrument<NELead6Voice>::getParameter(id);
  }
}


NELead6Knob::NELead6Knob(InstrumentParameter* p, const sf::Texture &texture, const sf::IntRect &backRect, const sf::IntRect &knobRect) :
Knob( p, texture, backRect, knobRect),
_selector(sf::Vector2f(17,8))
{
  overColor = sf::Color(255,255,255,255);
  _selector.setOrigin(60,4);
  _selector.setPosition(64,64);
}

NELead6Knob::~NELead6Knob()
{
  
}


void NELead6Knob::update()
{
  Knob::update();
  unsigned angle = _param->getValueToUnsigned(14);
  short val = _param->getValue();
  
  if (val) _selector.setFillColor(sf::Color(255,42,42,255));
  else _selector.setFillColor(sf::Color(42,255,42,255));
  
  _selector.setRotation(angle*19.f - 42.f);
  
}


void NELead6Knob::draw (sf::RenderTarget &target, sf::RenderStates states) const
{
  states.transform *= getTransform();
  target.draw(_back_sprite,states);
  target.draw(_knob_sprite,states);
  target.draw(_selector,states);
}

void NELead6::step(Signal* l, Signal* r)
{
  Instrument<NELead6Voice>::step(l,r);
}

NELead6Interface::NELead6Interface(NELead6* instrument, const sf::Vector2f& size):
Interface(sf::Vector2i(1792,360),size),
_texture(),
_back(),
_instrument(instrument),
_outputKnob(0),
_oscmixKnob(0),
_lfo1AmKnob(0),
_lfo1RateKnob(0),
_lfo2AmKnob(0),
_lfo2RateKnob(0),
_envAttackKnob(0),
_envDecayKnob(0),
_envSustainKnob(0),
_envReleaseKnob(0),
_osc1ShapeKnob(0),
_osc2ShapeKnob(0)
{
  if (_instrument && _texture.loadFromFile("img/nelead6.png"))
  {
    _back.setTexture(_texture);
    _back.setTextureRect(sf::IntRect(0,0,1792,360));

    _outputKnob =  new NELead6Knob(_instrument->getParameter(PARAM_INSTRUMENT_VOLUME_ID),
                                   _texture,
                                   sf::IntRect(1792,0,128,128),
                                   sf::IntRect(1792,128,128,128));
                                   
    _oscmixKnob  =  new NELead6Knob(_instrument->getParameter(PARAM_NELEAD6_OSCMIX),
                                   _texture,
                                   sf::IntRect(1792,0,128,128),
                                   sf::IntRect(1792,128,128,128));
                                   
    _lfo1AmKnob =  new NELead6Knob(_instrument->getParameter(PARAM_NELEAD6_LFO1AMOUNT),
                                   _texture,
                                   sf::IntRect(1792,0,128,128),
                                   sf::IntRect(1792,128,128,128));
    _lfo1RateKnob =  new NELead6Knob(_instrument->getParameter(PARAM_NELEAD6_LFO1RATE),
                                   _texture,
                                   sf::IntRect(1792,0,128,128),
                                   sf::IntRect(1792,128,128,128));

    _lfo2AmKnob =  new NELead6Knob(_instrument->getParameter(PARAM_NELEAD6_LFO2AMOUNT),
                                   _texture,
                                   sf::IntRect(1792,0,128,128),
                                   sf::IntRect(1792,128,128,128));

    _lfo2RateKnob =  new NELead6Knob(_instrument->getParameter(PARAM_NELEAD6_LFO2RATE),
                                   _texture,
                                   sf::IntRect(1792,0,128,128),
                                   sf::IntRect(1792,128,128,128));  
                                   
    _envAttackKnob =  new NELead6Knob(_instrument->getParameter(PARAM_NELEAD6_ENVATTACK),
                                   _texture,
                                   sf::IntRect(1792,0,128,128),
                                   sf::IntRect(1792,128,128,128));                                   
     _envDecayKnob =  new NELead6Knob(_instrument->getParameter(PARAM_NELEAD6_ENVDECAY),
                                   _texture,
                                   sf::IntRect(1792,0,128,128),
                                   sf::IntRect(1792,128,128,128));                                   
     _envSustainKnob =  new NELead6Knob(_instrument->getParameter(PARAM_NELEAD6_ENVSUSTAIN),
                                   _texture,
                                   sf::IntRect(1792,0,128,128),
                                   sf::IntRect(1792,128,128,128));                                   
     _envReleaseKnob =  new NELead6Knob(_instrument->getParameter(PARAM_NELEAD6_ENVRELEASE),
                                   _texture,
                                   sf::IntRect(1792,0,128,128),
                                   sf::IntRect(1792,128,128,128));


    _osc1ShapeKnob = new NELead6Knob(_instrument->getParameter(PARAM_NELEAD6_OSC1SHAPE),
                                      _texture,
                                      sf::IntRect(1792,0,128,128),
                                       sf::IntRect(1792,128,128,128));
    _osc2ShapeKnob = new NELead6Knob(_instrument->getParameter(PARAM_NELEAD6_OSC2SHAPE),
                                      _texture,
                                      sf::IntRect(1792,0,128,128),
                                      sf::IntRect(1792,128,128,128));
                                       
    sf::Vector2f scale(0.59f,0.59f);
    _outputKnob->setScale(scale); 
    _oscmixKnob->setScale(scale); 
    _lfo1AmKnob->setScale(scale);  
    _lfo1RateKnob->setScale(scale);
    _lfo2AmKnob->setScale(scale);
    _lfo2RateKnob->setScale(scale);
    _envAttackKnob->setScale(scale);
    _envDecayKnob->setScale(scale);
    _envSustainKnob->setScale(scale);
    _envReleaseKnob->setScale(scale);
    _osc1ShapeKnob->setScale(scale);
    _osc2ShapeKnob->setScale(scale);
    
    _outputKnob->setPosition(1146,30);
    _oscmixKnob->setPosition(644,230);
    _lfo1AmKnob->setPosition(362,26);
    _lfo1RateKnob->setPosition(106,26);
    _lfo2AmKnob->setPosition(362,126);    
    _lfo2RateKnob->setPosition(106,126);
    _envAttackKnob->setPosition(744,24);
    _envDecayKnob->setPosition(832,24);
    _envSustainKnob->setPosition(925,24);
    _envReleaseKnob->setPosition(1020,24);
    _osc1ShapeKnob->setPosition(456,130);
    _osc2ShapeKnob->setPosition(645,130);
    
    addMouseCatcher(_outputKnob);
    addMouseCatcher(_lfo1AmKnob);
    addMouseCatcher(_lfo1RateKnob);
    addMouseCatcher(_lfo2AmKnob);
    addMouseCatcher(_lfo2RateKnob);
    addMouseCatcher(_oscmixKnob);
    addMouseCatcher(_envAttackKnob);
    addMouseCatcher(_envDecayKnob);
    addMouseCatcher(_envSustainKnob);
    addMouseCatcher(_envReleaseKnob);
    addMouseCatcher(_osc1ShapeKnob);
    addMouseCatcher(_osc2ShapeKnob);
    addDrawable(&_back);
  }

}

NELead6Interface::~NELead6Interface()
{
  if( _outputKnob ) delete _outputKnob;
  if( _oscmixKnob ) delete _oscmixKnob;
  if( _lfo1AmKnob ) delete _lfo1AmKnob;
  if( _lfo1RateKnob ) delete _lfo1RateKnob;
  if( _lfo2AmKnob ) delete _lfo2AmKnob;
  if( _lfo2RateKnob ) delete _lfo2RateKnob;
  if(_envAttackKnob ) delete _envAttackKnob;
  if(_envDecayKnob ) delete _envDecayKnob;
  if(_envSustainKnob ) delete _envSustainKnob;
  if(_envReleaseKnob ) delete _envReleaseKnob;
}

