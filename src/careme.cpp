
#include "careme.hpp"

CaremeVoice::CaremeVoice(AbstractInstrument* creator):
InstrumentVoice(creator)
{
}

CaremeVoice::~CaremeVoice()
{
}

void CaremeVoice::beginNote(Note& n)
{
  _osc1.resetTime();
  _osc2.resetTime();
  _osc3.resetTime();
  _osc1.setFrequency(n.frequency());
  _osc1.setAmplitude(0.6);
  _osc2.setFrequency(2*n.frequency());
  _osc2.setAmplitude(0.4);
  _osc3.setFrequency(4*n.frequency());
  _osc3.setAmplitude(0.01);
  _used=true;
}

void CaremeVoice::endNote()
{
  _used=false;
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

  _osc1.step(output);
  output->add(_osc2.generate());
  output->add(_osc3.generate());
}
/*
Careme::Careme() :
Instrument<CaremeVoice>(),
_lfo_amp(0,0,100),
_lfo_frequency(0,0,100),
_duty(127,0,255)
{

}

Careme::~Careme()
{
}

InstrumentParameter* Careme::getParameter(unsigned char id)
{
  switch (id)
  {
    case PARAM_PURESQUARE_LFOFREQUENCY_ID: return &_lfo_frequency;
    case PARAM_PURESQUARE_LFOAMP_ID: return &_lfo_amp;
    case PARAM_PURESQUARE_DUTY_ID: return &_duty;
    default: return Instrument<CaremeVoice>::getParameter(id);
  }
}

CaremeInterface::CaremeInterface(Careme* instrument,const sf::Vector2f& size):
Interface(sf::Vector2i(720,360),size),
_texture(),
_back(),
_lfoAmpKnob(0),
_lfoFrequencyKnob(0),
_dutyKnob(0),
_instrument(instrument)
{
  if (_instrument && _texture.loadFromFile("img/puresquare.png"))
  {
    _lfoAmpKnob = new Knob(_instrument->getParameter(PARAM_PURESQUARE_LFOAMP_ID),
                           _texture,
                           sf::IntRect(720,0,128,128),
                           sf::IntRect(720,128,128,128));
                           
    _lfoFrequencyKnob = new Knob(_instrument->getParameter(PARAM_PURESQUARE_LFOFREQUENCY_ID),
                                 _texture,
                                 sf::IntRect(720,0,128,128),
                                 sf::IntRect(720,128,128,128));
                                 
    _dutyKnob = new Knob(_instrument->getParameter(PARAM_PURESQUARE_DUTY_ID),
                         _texture,
                         sf::IntRect(720,0,128,128),
                         sf::IntRect(720,128,128,128));
                         
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
    
    addDrawable(&_back);
  }
}

CaremeInterface::~CaremeInterface()
{
  if (_lfoAmpKnob) delete _lfoAmpKnob;
  if (_lfoFrequencyKnob) delete _lfoFrequencyKnob;
  if (_dutyKnob) delete _dutyKnob;
}
*/
