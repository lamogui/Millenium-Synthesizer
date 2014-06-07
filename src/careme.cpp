/*******************************************************
Nom ......... : careme.cpp
Role ........ : Instrument de Mr CHOI
Auteur ...... : Kwon-Young CHOI
Version ..... : V1.7 olol
Licence ..... : © Copydown™
********************************************************/
#include "careme.hpp"

#ifdef _MSC_VER
#pragma warning( disable : 4305 )
#endif

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

void CaremeVoice::step(Signal* leftout, Signal* rightout)
{

  _env.attack = _instrument->getParameter(PARAM_CAREME_ENVATTACK)->getValue()*Signal::frequency/80;
  _env.decay = _instrument->getParameter(PARAM_CAREME_ENVDECAY)->getValue()*Signal::frequency/80;
  _env.sustain = (float)_instrument->getParameter(PARAM_CAREME_ENVSUSTAIN)->getValue()/255.f;
  _env.release = _instrument->getParameter(PARAM_CAREME_ENVRELEASE)->getValue()*Signal::frequency/80;

  _osc1->step(leftout);
  leftout->add(_osc2->generate());
  leftout->add(_osc3->generate());
  leftout->mix(_env.generate());
  if (rightout) *rightout = *leftout;
  if (_env.hasEnded()) _used = false;
}

Careme::Careme() :
Instrument<CaremeVoice>(),
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
    default: return Instrument<CaremeVoice>::getParameter(id);
  }
}


CaremeInterface::CaremeInterface(Careme* instrument,const sf::Vector2f& size):
Interface(sf::Vector2u(1120,691),size),
_texture(),
_back(),
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

    _envAttackKnob =  new NELead6Knob(_instrument->getParameter(PARAM_CAREME_ENVATTACK),
                                      _texture,
                                      sf::IntRect(1120,0,128,128),
                                      sf::IntRect(1120,128,128,128));                                   
    _envDecayKnob =  new NELead6Knob( _instrument->getParameter(PARAM_CAREME_ENVDECAY),
                                      _texture,
                                      sf::IntRect(1120,0,128,128),
                                      sf::IntRect(1120,128,128,128));                                   
    _envSustainKnob =  new NELead6Knob( _instrument->getParameter(PARAM_CAREME_ENVSUSTAIN),
                                        _texture,
                                        sf::IntRect(1120,0,128,128),
                                        sf::IntRect(1120,128,128,128));                                   
    _envReleaseKnob =  new NELead6Knob( _instrument->getParameter(PARAM_CAREME_ENVRELEASE),
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
    
    addDrawable(&_back);
  }
}

CaremeInterface::~CaremeInterface()
{
  if(_envAttackKnob ) delete _envAttackKnob;
  if(_envDecayKnob ) delete _envDecayKnob;
  if(_envSustainKnob ) delete _envSustainKnob;
  if(_envReleaseKnob ) delete _envReleaseKnob;
}

