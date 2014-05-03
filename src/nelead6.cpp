#include "nelead6.hpp"
#include <cmath>

NELead6Voice::NELead6Voice(AbstractInstrument* creator) :
InstrumentVoice(creator),
_osc1(new SawOscillator),
_osc2(new SawOscillator),
_lfo1(new TriangleOscillator),
_lfo2(new TriangleOscillator),
_filter1(new LowPassFilter2),
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
  
  /*_osc1->setFrequency(_currentNote.frequency());
  _osc1->setAmplitude(_currentNote.velocity);
  _osc2->setFrequency(_currentNote.frequency());
  _osc2->setAmplitude(_currentNote.velocity);*/
  
  _used=true;
}

void NELead6Voice::endNote()
{
  _env.beginRelease();
}

void NELead6Voice::step(Signal* leftout, Signal* rightout)
{

  const float oscmix = _instrument->getParameter(PARAM_NELEAD6_OSCMIX)->getValue()/127.f;
  const float oscmod = _instrument->getParameter(PARAM_NELEAD6_OSCMOD)->getValue()/(127.f*440.f*10.f);
  
  const int finetune = _instrument->getParameter(PARAM_NELEAD6_FINETUNE)->getValue();
  const int coarsetune = _instrument->getParameter(PARAM_NELEAD6_COARSETUNE)->getValue();
  
  const float lfo1_rate = exp(_instrument->getParameter(PARAM_NELEAD6_LFO1RATE)->getValue()*9.70f/127.f - 3.50f);
  const float lfo2_rate = exp(_instrument->getParameter(PARAM_NELEAD6_LFO2RATE)->getValue()*9.70f/127.f - 3.50f);
  
  const float lfo1_amount = _instrument->getParameter(PARAM_NELEAD6_LFO1AMOUNT)->getValue()/127.f;
  const float lfo2_amount = _instrument->getParameter(PARAM_NELEAD6_LFO2AMOUNT)->getValue()/127.f;
  
  const float osc1_shape = _instrument->getParameter(PARAM_NELEAD6_OSC1SHAPE)->getValue()/127.f;
  const float osc2_shape = _instrument->getParameter(PARAM_NELEAD6_OSC2SHAPE)->getValue()/127.f;
  
  const float filter1_rate = exp(_instrument->getParameter(PARAM_NELEAD6_FILTER1RATE)->getValue()*7.423f/127.f + 2.48);
  const float filter1_res = 50.f/(float)(_instrument->getParameter(PARAM_NELEAD6_FILTER1RES)->getValue());
  
  _env.attack = _instrument->getParameter(PARAM_NELEAD6_ENVATTACK)->getValue()*Signal::frequency/40;
  _env.decay = _instrument->getParameter(PARAM_NELEAD6_ENVDECAY)->getValue()*Signal::frequency/40;
  _env.sustain = (float)_instrument->getParameter(PARAM_NELEAD6_ENVSUSTAIN)->getValue()/127.f;
  _env.release = _instrument->getParameter(PARAM_NELEAD6_ENVRELEASE)->getValue()*Signal::frequency/40;
  
  
  _filter1->setFrequency(filter1_rate*_currentNote.frequency()/440.f);
  _filter1->setResonance(filter1_res);
  
  _lfo1->setAmplitude(lfo1_amount);
  _lfo2->setAmplitude(lfo2_amount);
  
  
  //begin with LFO
  _lfo1->setFrequency(lfo1_rate*_currentNote.frequency()/440.f);
  _lfo2->setFrequency(lfo2_rate*_currentNote.frequency()/440.f);
  
  Signal* lfo1 = _lfo1->generate();
  Signal* lfo2 = _lfo2->generate(); 
  
  lfo2->scale(0.5f);
  
  _osc1->setAmplitude(_currentNote.velocity);
  _osc1->setFrequency(_currentNote.frequency());
  _osc1->setShape(osc1_shape);
  _osc2->setShape(osc2_shape);
  _osc2->getShape().add(lfo2);
  _osc2->getShape().saturate(0.f,1.f);
  
  const int id = _currentNote.id + coarsetune;
  float osc2f = Note::getFrequencyFromID(id);
  if (finetune > 0)
  {
    osc2f += (Note::getFrequencyFromID(id+1) - osc2f)*finetune/128.f;
  }
  else if (finetune < 0)
  {
    osc2f += (osc2f - Note::getFrequencyFromID(id))*finetune/128.f;
  }
  _osc2->setFrequency(osc2f);
  _osc2->setAmplitude(_currentNote.velocity);
  
  
  _oscmix.constant(oscmix);
  lfo1->scale(0.5f);
  _oscmix.add(lfo1);
  _oscmix.saturate(0.f,1.f);
  
  
  Signal* osc2 = _osc2->generate();
  _osc1->getFM()=*osc2;
  _osc1->getFM().scale(_currentNote.frequency()*oscmod);
  _osc1->getFM().addOffset(_currentNote.frequency()*oscmod);
  _osc1->step(leftout);
  osc2->mix(&_oscmix);
  
  _oscmix.scale(-1.f);
  _oscmix.addOffset(1.f);
  
  leftout->mix(&_oscmix);
  
  leftout->add(osc2);
  
  _filter1->step(leftout);
  
  leftout->mix(_env.generate());
  
  
  
  if (rightout)
    *rightout = *leftout;
    
  
  
  if (_env.hasEnded()) {
    _used = false;
    _instrument->getParameter(PARAM_NELEAD6_OSCMIX)->setAuto(false,0);
    _instrument->getParameter(PARAM_NELEAD6_OSC1SHAPE)->setAuto(false,0);
    _instrument->getParameter(PARAM_NELEAD6_OSC2SHAPE)->setAuto(false,0);
  }
  else if (visualize)
  {
    _instrument->getParameter(PARAM_NELEAD6_OSCMIX)->setAuto(true,(1.f-_oscmix.samples[0])*127.f);
    _instrument->getParameter(PARAM_NELEAD6_OSC1SHAPE)->setAuto(true,_osc1->getShape().samples[0]*127.f);
    _instrument->getParameter(PARAM_NELEAD6_OSC2SHAPE)->setAuto(true,_osc2->getShape().samples[0]*127.f);
  }
}

NELead6::NELead6() :
Instrument<NELead6Voice>(),
_oscmix(0,0,127),
_oscmod(0,0,127),
_finetune(0,-64,64),
_coarsetune(0,-24,24),
_lfo1_amount(0,0,127),
_lfo1_rate(0,0,127),
_lfo2_amount(0,0,127),
_lfo2_rate(0,0,127),
_env_attack(100,0,127),
_env_decay(50,0,127),
_env_sustain(100,0,127),
_env_release(80,0,127),
_osc1_shape(0,0,127),
_osc2_shape(0,0,127),
_filter1_rate(64,0,127),
_filter1_resonance(50,50,400),
_osc1_type(1,1,6),
_osc2_type(1,1,6),
_lfo1_type(1,1,6),
_lfo2_type(1,1,6),
_filter_type(1,1,6)
{
  _osc1_type.notifyOnChange(this);
  _osc2_type.notifyOnChange(this);
  _lfo1_type.notifyOnChange(this);
  _lfo2_type.notifyOnChange(this);
  _filter_type.notifyOnChange(this);
}
 
NELead6::~NELead6()
{
}

void NELead6::notify(InstrumentParameter* p)
{
  if (p==&_osc1_type)
  {
    switch (p->getValue())
    {
      case 1:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceOsc1(new SawOscillator);
        break;
      case 2:
        _osc1_shape.setValue(64);
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceOsc1(new SquareOscillator);
        break;
      case 3:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceOsc1(new TriangleOscillator);
        break;
      case 4:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceOsc1(new SinusoidalOscillator);
        break;
      case 5:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceOsc1(new WhiteNoiseOscillator);
        break;
      case 6:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceOsc1(new TriplePeakOscillator);
        break;
    }
  }
  else if (p==&_osc2_type)
  {
    switch (p->getValue())
    {
      case 1:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceOsc2(new SawOscillator);
        break;
      case 2:
        _osc2_shape.setValue(64);
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceOsc2(new SquareOscillator);
        break;
      case 3:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceOsc2(new TriangleOscillator);
        break;
      case 4:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceOsc2(new SinusoidalOscillator);
        break;
      case 5:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceOsc2(new WhiteNoiseOscillator);
        break;
      case 6:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceOsc2(new TriplePeakOscillator);
        break;
    }
  }
  else if (p==&_lfo1_type)
  {
    switch (p->getValue())
    {
      case 1:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceLfo1(new TriangleOscillator);
        break;
      case 2:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceLfo1(new SawOscillator);
        break;
      case 3:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceLfo1(new SquareOscillator);
        break;
      case 4:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceLfo1(new RandomOscillator);
        break;
      case 5:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceLfo1(new RandomSmoothOscillator);
        break;
      case 6:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceLfo1(new TriplePeakOscillator);
        break;
    }
  }
  else if (p==&_lfo2_type)
  {
    switch (p->getValue())
    {
      case 1:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceLfo2(new TriangleOscillator);
        break;
      case 2:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceLfo2(new SawOscillator);
        break;
      case 3:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceLfo2(new SquareOscillator);
        break;
      case 4:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceLfo2(new RandomOscillator);
        break;
      case 5:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceLfo2(new RandomSmoothOscillator);
        break;
      case 6:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceLfo2(new TriplePeakOscillator);
        break;
    }
  }
  else if (p==&_filter_type)
  {
    switch (p->getValue())
    {
      case 1:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceFilter(new LowPassFilter2);
        break;
      case 2:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceFilter(new BandPassFilter2);
        break;
      case 3:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceFilter(new HighPassFilter2);
        break;
      case 4:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceFilter(new Rejector);
        break;
      case 5:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceFilter(new LowPassFilter);
        break;
      case 6:
        for (unsigned int i=0; i < _voices.size(); i++) _voices[i]->replaceFilter(new HighPassFilter);
        break;
    }
  }
  else 
  {
    return;
  }
  
  p->valueUsed();
}

InstrumentParameter* NELead6::getParameter(unsigned char id)
{
  switch (id)
  {
    case PARAM_NELEAD6_OSCMIX: return &_oscmix;
    case PARAM_NELEAD6_OSCMOD: return &_oscmod; 
    case PARAM_NELEAD6_FINETUNE: return &_finetune; 
    case PARAM_NELEAD6_COARSETUNE: return &_coarsetune; 
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
    case PARAM_NELEAD6_FILTER1RATE: return &_filter1_rate;
    case PARAM_NELEAD6_FILTER1RES: return &_filter1_resonance;
    case PARAM_NELEAD6_OSC1TYPE: return &_osc1_type;
    case PARAM_NELEAD6_OSC2TYPE: return &_osc2_type;
    case PARAM_NELEAD6_LFO1TYPE: return &_lfo1_type;
    case PARAM_NELEAD6_LFO2TYPE: return &_lfo2_type;
    case PARAM_NELEAD6_FILTERTYPE: return &_filter_type;
    default : return Instrument<NELead6Voice>::getParameter(id);
  }
}


NELead6Knob::NELead6Knob(InstrumentParameter* p, const sf::Texture &texture, const sf::IntRect &backRect, const sf::IntRect &knobRect) :
Knob( p, texture, backRect, knobRect),
_selector(sf::Vector2f(17.5f,8.5f)),
_autoSelector(sf::Vector2f(17.5f,8))
{
  overColor = sf::Color(255,255,255,255);
  _selector.setOrigin(60,4.1f);
  _selector.setPosition(64,64);
  _autoSelector.setOrigin(60,4.1f);
  _autoSelector.setPosition(64,64);
  _autoSelector.setFillColor(sf::Color(30,74,193,255));
}

NELead6Knob::~NELead6Knob()
{
  
}


void NELead6Knob::update()
{
  Knob::update();
  if (_param) {
    unsigned angle = _param->getValueToUnsigned(14);
    short val = _param->getValue();
    
    if (val) _selector.setFillColor(sf::Color(255,42,42,255));
    else _selector.setFillColor(sf::Color(42,255,42,255));
    
    _selector.setRotation(angle*18.9f - 43.f);
    if (_param->isAuto()){
      unsigned auto_angle = _param->getAutoToUnsigned(14);
      _autoSelector.setRotation(auto_angle*18.9f - 43.f);
    }
  }
}


void NELead6Knob::draw (sf::RenderTarget &target, sf::RenderStates states) const
{
  states.transform *= getTransform();
  target.draw(_back_sprite,states);
  target.draw(_knob_sprite,states);
  if (!_catched && _param && _param->isAuto())
    target.draw(_autoSelector,states);
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
_osc1TypeLED(0),
_osc2TypeLED(0),
_lfo1TypeLED(0),
_lfo2TypeLED(0),
_outputKnob(0),
_oscmixKnob(0),
_oscmodKnob(0),
_finetuneKnob(0),
_coarsetuneKnob(0),
_lfo1AmKnob(0),
_lfo1RateKnob(0),
_lfo2AmKnob(0),
_lfo2RateKnob(0),
_envAttackKnob(0),
_envDecayKnob(0),
_envSustainKnob(0),
_envReleaseKnob(0),
_osc1ShapeKnob(0),
_osc2ShapeKnob(0),
_filter1RateKnob(0),
_filter1ResKnob(0),
_osc1TypeButton(0),
_osc2TypeButton(0),
_lfo1TypeButton(0),
_lfo2TypeButton(0)
{
  if (_instrument && _texture.loadFromFile("img/nelead6.png"))
  {
    _back.setTexture(_texture);
    _back.setTextureRect(sf::IntRect(0,0,1792,360));
    
    _osc1TypeLED = new NELead6TriangleLED(_instrument->getParameter(PARAM_NELEAD6_OSC1TYPE), 
                                          _texture, 
                                          sf::IntRect(1840,256,22,43));
    _osc2TypeLED = new NELead6TriangleLED(_instrument->getParameter(PARAM_NELEAD6_OSC2TYPE), 
                                          _texture, 
                                          sf::IntRect(1840,256,22,43));
    _lfo1TypeLED = new NELead6TriangleLED(_instrument->getParameter(PARAM_NELEAD6_LFO1TYPE), 
                                          _texture, 
                                          sf::IntRect(1840,256,22,43));
    _lfo2TypeLED = new NELead6TriangleLED(_instrument->getParameter(PARAM_NELEAD6_LFO2TYPE), 
                                          _texture, 
                                          sf::IntRect(1840,256,22,43));
    _filterTypeLED = new NELead6TriangleLED(_instrument->getParameter(PARAM_NELEAD6_FILTERTYPE), 
                                          _texture, 
                                          sf::IntRect(1840,256,22,43));     

    _outputKnob =  new NELead6Knob(_instrument->getParameter(PARAM_INSTRUMENT_VOLUME_ID),
                                   _texture,
                                   sf::IntRect(1792,0,128,128),
                                   sf::IntRect(1792,128,128,128));
                                   
    _oscmixKnob  =  new NELead6Knob(_instrument->getParameter(PARAM_NELEAD6_OSCMIX),
                                   _texture,
                                   sf::IntRect(1792,0,128,128),
                                   sf::IntRect(1792,128,128,128));
                                   
    _oscmodKnob = new NELead6Knob(_instrument->getParameter(PARAM_NELEAD6_OSCMOD),
                                   _texture,
                                   sf::IntRect(1792,0,128,128),
                                   sf::IntRect(1792,128,128,128));
                                   
                                   
    _finetuneKnob = new NELead6Knob(_instrument->getParameter(PARAM_NELEAD6_FINETUNE),
                                   _texture,
                                   sf::IntRect(1792,0,128,128),
                                   sf::IntRect(1792,128,128,128));
      
    _coarsetuneKnob = new NELead6Knob(_instrument->getParameter(PARAM_NELEAD6_COARSETUNE),
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
                                      
                                      
    _filter1RateKnob = new NELead6Knob(_instrument->getParameter(PARAM_NELEAD6_FILTER1RATE),
                                      _texture,
                                      sf::IntRect(1792,0,128,128),
                                      sf::IntRect(1792,128,128,128));
                                      
    _filter1ResKnob = new NELead6Knob(_instrument->getParameter(PARAM_NELEAD6_FILTER1RES),
                                      _texture,
                                      sf::IntRect(1792,0,128,128),
                                      sf::IntRect(1792,128,128,128));
                                      
    _osc1TypeButton = new Button(_instrument->getParameter(PARAM_NELEAD6_OSC1TYPE),
                                 _texture,
                                 sf::IntRect(1792,256,48,26),
                                 sf::IntRect(1792,282,48,26),
                                 ButtonMode::increment);
                                 
    _osc2TypeButton = new Button(_instrument->getParameter(PARAM_NELEAD6_OSC2TYPE),
                                 _texture,
                                 sf::IntRect(1792,256,48,26),
                                 sf::IntRect(1792,282,48,26),
                                 ButtonMode::increment);
                                 
                                 
    _lfo1TypeButton = new Button(_instrument->getParameter(PARAM_NELEAD6_LFO1TYPE),
                                 _texture,
                                 sf::IntRect(1792,256,48,26),
                                 sf::IntRect(1792,282,48,26),
                                 ButtonMode::increment);
                                 
    _lfo2TypeButton = new Button(_instrument->getParameter(PARAM_NELEAD6_LFO2TYPE),
                                 _texture,
                                 sf::IntRect(1792,256,48,26),
                                 sf::IntRect(1792,282,48,26),
                                 ButtonMode::increment);
                                 
    _filterTypeButton = new Button(_instrument->getParameter(PARAM_NELEAD6_FILTERTYPE),
                                 _texture,
                                 sf::IntRect(1792,256,48,26),
                                 sf::IntRect(1792,282,48,26),
                                 ButtonMode::increment);
                                       
    sf::Vector2f scale(0.59f,0.59f);
    _outputKnob->setScale(scale); 
    _oscmixKnob->setScale(scale); 
    _oscmodKnob->setScale(scale);
    _finetuneKnob->setScale(scale);
    _coarsetuneKnob->setScale(scale);
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
    _filter1RateKnob->setScale(scale);
    _filter1ResKnob->setScale(scale);
    
    _osc1TypeLED->setPosition(482,32);
    _osc2TypeLED->setPosition(680,32);
    _lfo1TypeLED->setPosition(216,24);
    _lfo2TypeLED->setPosition(216,126);
    _filterTypeLED->setPosition(864,228);
    _outputKnob->setPosition(1146,30);
    _oscmixKnob->setPosition(644,230);
    _oscmodKnob->setPosition(458,230);
    _finetuneKnob->setPosition(552,178);
    _coarsetuneKnob->setPosition(552,92);
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
    _filter1RateKnob->setPosition(738,232);
    _filter1ResKnob->setPosition(936,232);
    _osc1TypeButton->setPosition(470,78);
    _osc2TypeButton->setPosition(668,78);
    _lfo1TypeButton->setPosition(202,68);
    _lfo2TypeButton->setPosition(202,170);
    _filterTypeButton->setPosition(854,274);
    
    addMouseCatcher(_osc1TypeLED);
    addMouseCatcher(_osc2TypeLED);
    addMouseCatcher(_lfo1TypeLED);
    addMouseCatcher(_lfo2TypeLED);
    addMouseCatcher(_filterTypeLED);
    addMouseCatcher(_outputKnob);
    addMouseCatcher(_oscmixKnob);
    addMouseCatcher(_oscmodKnob);
    addMouseCatcher(_finetuneKnob);
    addMouseCatcher(_coarsetuneKnob);
    addMouseCatcher(_lfo1AmKnob);
    addMouseCatcher(_lfo1RateKnob);
    addMouseCatcher(_lfo2AmKnob);
    addMouseCatcher(_lfo2RateKnob);
    addMouseCatcher(_envAttackKnob);
    addMouseCatcher(_envDecayKnob);
    addMouseCatcher(_envSustainKnob);
    addMouseCatcher(_envReleaseKnob);
    addMouseCatcher(_osc1ShapeKnob);
    addMouseCatcher(_osc2ShapeKnob);
    addMouseCatcher(_filter1RateKnob);
    addMouseCatcher(_filter1ResKnob);
    addMouseCatcher(_osc1TypeButton);
    addMouseCatcher(_osc2TypeButton);
    addMouseCatcher(_lfo1TypeButton);
    addMouseCatcher(_lfo2TypeButton);
    addMouseCatcher(_filterTypeButton);
    
    addDrawable(&_back);
  }

}

NELead6Interface::~NELead6Interface()
{
  if (_osc1TypeLED) delete _osc1TypeLED;
  if (_osc2TypeLED) delete _osc2TypeLED;
  if (_lfo1TypeLED) delete _lfo1TypeLED;
  if (_lfo2TypeLED) delete _lfo2TypeLED;
  if (_filterTypeLED) delete _filterTypeLED;
  if( _outputKnob ) delete _outputKnob;
  if( _oscmixKnob ) delete _oscmixKnob;
  if (_oscmodKnob) delete _oscmodKnob;
  if (_finetuneKnob ) delete _finetuneKnob;
  if (_coarsetuneKnob ) delete _coarsetuneKnob;
  if( _lfo1AmKnob ) delete _lfo1AmKnob;
  if( _lfo1RateKnob ) delete _lfo1RateKnob;
  if( _lfo2AmKnob ) delete _lfo2AmKnob;
  if( _lfo2RateKnob ) delete _lfo2RateKnob;
  if(_envAttackKnob ) delete _envAttackKnob;
  if(_envDecayKnob ) delete _envDecayKnob;
  if(_envSustainKnob ) delete _envSustainKnob;
  if(_envReleaseKnob ) delete _envReleaseKnob;
  if(_osc1ShapeKnob ) delete _osc1ShapeKnob;
  if(_osc2ShapeKnob ) delete _osc2ShapeKnob;
  if(_filter1RateKnob ) delete _filter1RateKnob;
  if(_filter1ResKnob ) delete _filter1ResKnob;
  if (_osc1TypeButton) delete _osc1TypeButton;
  if (_osc2TypeButton) delete _osc2TypeButton;
  if (_lfo1TypeButton) delete _lfo1TypeButton;
  if (_lfo2TypeButton) delete _lfo2TypeButton;
  if (_filterTypeButton) delete _filterTypeButton;
}

NELead6TriangleLED::NELead6TriangleLED(InstrumentParameter* p, const sf::Texture &texture, const sf::IntRect &rect) :
_param(p),
_sprite(),
_light()
{
  _sprite.setTexture(texture);
  _sprite.setTextureRect(rect);
  _light.setOrigin(11,0);
  _light.setFillColor(sf::Color(42,255,42,255));
  _light.setPointCount(3);
  _light.setPoint(0, sf::Vector2f(9, 0));
  _light.setPoint(1, sf::Vector2f(9, 8));
  _light.setPoint(2, sf::Vector2f(2, 4));
  
  update();
}

NELead6TriangleLED::~NELead6TriangleLED()
{

}


void NELead6TriangleLED::update()
{
  if (_param  )
  {
    if (_param->getMax() <= 6) {
      switch (_param->getValue())
      {
        case 1:
        case 6:
          _light.setPosition(11,32);
          break;
        case 2:
        case 5:
          _light.setPosition(11,17.5);
          break;
        case 3:
        case 4:
          _light.setPosition(11,2.5f);
        break;
      }
      if (_param->getValue()<4)
        _light.setScale(1.f,1.f);
      else
        _light.setScale(-1.f,1.f);
    }
  }
}

bool NELead6TriangleLED::onMousePress(float x, float y)
{
  if (_param  && _param->getMax() <= 6)
  {
    sf::Vector2f v(getInverseTransform().transformPoint(x,y));
    
    if (v.x > 22 ||
        v.x < 0 ||
        v.y > 42 ||
        v.y < 0 ) 
        return false;  

    return true;
  }
  return false;
}

void NELead6TriangleLED::onMouseMove(float x, float y)
{
}

void NELead6TriangleLED::onMouseRelease(float x, float y)
{
  if (_param  && _param->getMax() <= 6)
  {
    sf::Vector2f v(getInverseTransform().transformPoint(x,y));
    if (v.y > 0 && v.x > 0) {
      if (v.x < 12)
      {
        if (v.y < 14) _param->setValue(3);
        else if (v.y < 29) _param->setValue(2);
        else if (v.y < 42) _param->setValue(1);
      } 
      else if(v.x < 22)
      {
        if (v.y < 14) _param->setValue(4);
        else if (v.y < 29) _param->setValue(5);
        else if (v.y < 42) _param->setValue(6);
      }
    }
  }
}

void NELead6TriangleLED::draw (sf::RenderTarget &target, sf::RenderStates states) const
{
  states.transform *= getTransform();
  target.draw(_sprite,states);
  if (_param)
    target.draw(_light,states);
}


