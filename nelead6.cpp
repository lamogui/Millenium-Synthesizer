#include "nelead6.hpp"

NELead6Voice::NELead6Voice() :
_osc1(new SinusoidalOscillator),
_osc2(new SinusoidalOscillator),
_lfo1(new SinusoidalOscillator),
_lfo2(new SinusoidalOscillator),
_modulation_mode(NELEAD6_FM)
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
  if (_modulation_mode==NELEAD6_RM)
  {
    _osc1->step(output);
    output->mix(&_osc2->generate());
  }
  else
  {
    _osc2->step(_osc1->fm);
    _osc1->step(output);
  }
}

