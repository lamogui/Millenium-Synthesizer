#include "nelead6.hpp"

NELead6Voice::NELead6Voice(AbstractInstrument* creator) :
InstrumentVoice(creator),
_osc1(new SquareOscillator),
_osc2(new SinusoidalOscillator),
_lfo1(new SinusoidalOscillator),
_lfo2(new SinusoidalOscillator)
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
  _osc1->resetTime();
  _osc2->resetTime();
  _lfo1->resetTime();
  _lfo2->resetTime();
  
  _osc1->setFrequency(n.frequency);
  _osc1->setAmplitude(n.velocity);
  _osc2->setFrequency(n.frequency*0.25);
  _osc2->setAmplitude(0.5);

  _used=true;
}

void NELead6Voice::endNote()
{
  _used=false;
}

void NELead6Voice::step(Signal* output)
{
  _osc1->step(output);
}

NELead6::NELead6() :
Instrument()
 {
 }
 
NELead6::~NELead6()
{
}

InstrumentParameter* NELead6::getParameter(unsigned char id)
{
  return Instrument<NELead6Voice>::getParameter(id);
}

void NELead6::step(Signal* output)
{
  Instrument<NELead6Voice>::step(output);
}

NELead6Interface::NELead6Interface(NELead6* instrument, const sf::Vector2f& size):
Interface(sf::Vector2i(1792,360),size),
_texture(),
_back(),
_instrument(instrument),
_outputKnob(0)
{
  if (_instrument && _texture.loadFromFile("img/nelead6.png"))
  {
    _back.setTexture(_texture);
    _back.setTextureRect(sf::IntRect(0,0,1792,360));

    _outputKnob =  new Knob(_instrument->getParameter(PARAM_INSTRUMENT_VOLUME_ID),
                           _texture,
                           sf::IntRect(1792,0,128,128),
                           sf::IntRect(1792,128,128,128));
                           
    _outputKnob->setScale(0.6f,0.6f);     
    _outputKnob->overColor = sf::Color(255,255,255,255);
    _outputKnob->setPosition(1146,30);
    
    addMouseCatcher(_outputKnob);
    addDrawable(&_back);
  }

}

NELead6Interface::~NELead6Interface()
{
  if( _outputKnob ) delete _outputKnob;

}

