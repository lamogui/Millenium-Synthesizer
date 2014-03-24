
#include "careme.hpp"

CaremeVoice::CaremeVoice(AbstractInstrument* creator):
InstrumentVoice(creator),
_osc1(new SinusoidalOscillator),
_osc2(new SinusoidalOscillator),
_osc3(new SinusoidalOscillator)
{
}

CaremeVoice::~CaremeVoice()
{
  delete _osc1;
  delete _osc2;
  delete _osc3;
}

void CaremeVoice::beginNote(Note& n)
{
  _osc1->resetTime();
  _osc2->resetTime();
  _osc3->resetTime();
  _env.resetTime();
  _osc1->setFrequency(n.frequency());
  _osc1->setAmplitude(0.6);
  _osc2->setFrequency(2*n.frequency());
  _osc2->setAmplitude(0.4);
  _osc3->setFrequency(4*n.frequency());
  _osc3->setAmplitude(0.01);
  _used=true;
}

void CaremeVoice::endNote()
{
  _env.beginRelease();
}

void CaremeVoice::step(Signal* output)
{
  /*
  float lfo_f = _instrument->getParameter(PARAM_PURESQUARE_LFOFREQUENCY_ID)->getValue()/10.f;
  float lfo_a = _instrument->getParameter(PARAM_PURESQUARE_LFOAMP_ID)->getValue()/255.f;
  float duty = _instrument->getParameter(PARAM_PURESQUARE_DUTY_ID)->getValue()/255.f;
  _lfo.setFrequency(lfo_f);
  _lfo.setAmplitude(lfo_a);
  
  Signal* signal_duty = _lfo.generate();
  
  _osc.setShape(duty);
  _osc.getShape().add(signal_duty);
  */

  _env.attack = _instrument->getParameter(PARAM_CAREME_ENVATTACK)->getValue()*Signal::frequency/80;
  _env.decay = _instrument->getParameter(PARAM_CAREME_ENVDECAY)->getValue()*Signal::frequency/80;
  _env.sustain = (float)_instrument->getParameter(PARAM_CAREME_ENVSUSTAIN)->getValue()/255.f;
  _env.release = _instrument->getParameter(PARAM_CAREME_ENVRELEASE)->getValue()*Signal::frequency/80;

  _osc1->step(output);
  output->add(_osc2->generate());
  output->add(_osc3->generate());
  output->mix(_env.generate());
  if (_env.hasEnded()) _used = false;
}

Careme::Careme() :
Instrument<CaremeVoice>(),
  /*
_lfo_amp(0,0,100),
_lfo_frequency(0,0,100),
_duty(127,0,255)
*/
_env_attack(100,0,255),
_env_decay(50,0,255),
_env_sustain(200,0,255),
_env_release(80,0,255)
{

}

Careme::~Careme()
{
}

InstrumentParameter* Careme::getParameter(unsigned char id)
{
  switch (id)
  {
    case PARAM_CAREME_ENVATTACK: return &_env_attack;
    case PARAM_CAREME_ENVDECAY: return &_env_decay;
    case PARAM_CAREME_ENVSUSTAIN: return &_env_sustain;
    case PARAM_CAREME_ENVRELEASE: return &_env_release;
    /*
    case PARAM_PURESQUARE_LFOFREQUENCY_ID: return &_lfo_frequency;
    case PARAM_PURESQUARE_LFOAMP_ID: return &_lfo_amp;
    case PARAM_PURESQUARE_DUTY_ID: return &_duty;
    */
    default: return Instrument<CaremeVoice>::getParameter(id);
  }
}

CaremeKnob::CaremeKnob(InstrumentParameter* p, const sf::Texture &texture, const sf::IntRect &backRect, const sf::IntRect &knobRect) :
Knob( p, texture, backRect, knobRect),
_selector(sf::Vector2f(17,8))
{
  overColor = sf::Color(255,255,255,255);
  _selector.setOrigin(60,4);
  _selector.setPosition(64,64);
}

CaremeKnob::~CaremeKnob()
{
  
}

void CaremeKnob::update()
{
  Knob::update();
  unsigned angle = _param->getValueToUnsigned(14);
  short val = _param->getValue();
  
  if (val) _selector.setFillColor(sf::Color(255,42,42,255));
  else _selector.setFillColor(sf::Color(42,255,42,255));
  
  _selector.setRotation(angle*19.f - 42.f);
  
}


void CaremeKnob::draw (sf::RenderTarget &target, sf::RenderStates states) const
{
  states.transform *= getTransform();
  target.draw(_back_sprite,states);
  target.draw(_knob_sprite,states);
  target.draw(_selector,states);
}

void Careme::step(Signal* output)
{
  Instrument<CaremeVoice>::step(output);
}

CaremeInterface::CaremeInterface(Careme* instrument,const sf::Vector2f& size):
Interface(sf::Vector2i(1120,691),size),
_texture(),
_back(),
  /*
_lfoAmpKnob(0),
_lfoFrequencyKnob(0),
_dutyKnob(0),
*/
_instrument(instrument),
_envAttackKnob(0),
_envDecayKnob(0),
_envSustainKnob(0),
  _envReleaseKnob(0)
{
  if (_instrument && _texture.loadFromFile("img/careme.png"))
  {
    _back.setTexture(_texture);
    _back.setTextureRect(sf::IntRect(0,0,1120,691));

    _envAttackKnob =  new CaremeKnob(_instrument->getParameter(PARAM_CAREME_ENVATTACK),
                                      _texture,
                                      sf::IntRect(1120,0,128,128),
                                      sf::IntRect(1120,128,128,128));                                   
    _envDecayKnob =  new CaremeKnob( _instrument->getParameter(PARAM_CAREME_ENVDECAY),
                                      _texture,
                                      sf::IntRect(1120,0,128,128),
                                      sf::IntRect(1120,128,128,128));                                   
    _envSustainKnob =  new CaremeKnob( _instrument->getParameter(PARAM_CAREME_ENVSUSTAIN),
                                        _texture,
                                        sf::IntRect(1120,0,128,128),
                                        sf::IntRect(1120,128,128,128));                                   
    _envReleaseKnob =  new CaremeKnob( _instrument->getParameter(PARAM_CAREME_ENVRELEASE),
                                        _texture,
                                        sf::IntRect(1120,0,128,128),
                                        sf::IntRect(1120,128,128,128)); 


    sf::Vector2f scale(0.6f,0.6f);
    _envAttackKnob->setScale(scale);
    _envDecayKnob->setScale(scale);
    _envSustainKnob->setScale(scale);
    _envReleaseKnob->setScale(scale);

    _envAttackKnob->setPosition(744,24);
    _envDecayKnob->setPosition(832,24);
    _envSustainKnob->setPosition(925,24);
    _envReleaseKnob->setPosition(1020,24);

    addMouseCatcher(_envAttackKnob);
    addMouseCatcher(_envDecayKnob);
    addMouseCatcher(_envSustainKnob);
    addMouseCatcher(_envReleaseKnob);
    /*
    _lfoAmpKnob->setScale(0.5f,0.5f);
    _lfoFrequencyKnob->setScale(0.5f,0.5f);
    _lfoAmpKnob->setPosition(116,148);
    _lfoFrequencyKnob->setPosition(206,148);
    _dutyKnob->setPosition(470,108);

    _back.setTexture(_texture);
    _back.setTextureRect(sf::IntRect(0,0,720,360));

    addMouseCatcher(_lfoAmpKnob);
    addMouseCatcher(_lfoFrequencyKnob);
    addMouseCatcher(_dutyKnob);
    */
    addDrawable(&_back);
  }
}

CaremeInterface::~CaremeInterface()
{
  if(_envAttackKnob ) delete _envAttackKnob;
  if(_envDecayKnob ) delete _envDecayKnob;
  if(_envSustainKnob ) delete _envSustainKnob;
  if(_envReleaseKnob ) delete _envReleaseKnob;
  /*
  if (_lfoAmpKnob) delete _lfoAmpKnob;
  if (_lfoFrequencyKnob) delete _lfoFrequencyKnob;
  if (_dutyKnob) delete _dutyKnob;
  */
}
