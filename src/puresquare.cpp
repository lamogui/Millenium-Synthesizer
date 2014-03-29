
#include "puresquare.hpp"

PureSquareVoice::PureSquareVoice(AbstractInstrument* creator):
InstrumentVoice(creator)
{


}

PureSquareVoice::~PureSquareVoice()
{
  
}

void PureSquareVoice::beginNote(Note& n)
{
  _osc.resetTime();
  _lfo.resetTime();
  _osc.setFrequency(n.frequency());
  _osc.setAmplitude(n.velocity);
  _used=true;
}

void PureSquareVoice::endNote()
{
  _used=false;
  if (visualize)
  {
    _instrument->getParameter(PARAM_PURESQUARE_DUTY_ID)->setAuto(false,0);
  }
}

void PureSquareVoice::step(Signal* leftout, Signal* rightout)
{
  float lfo_f = _instrument->getParameter(PARAM_PURESQUARE_LFOFREQUENCY_ID)->getValue()/10.f;
  float lfo_a = _instrument->getParameter(PARAM_PURESQUARE_LFOAMP_ID)->getValue()/150.f;
  float duty = _instrument->getParameter(PARAM_PURESQUARE_DUTY_ID)->getValue()/255.f;
  _lfo.setFrequency(lfo_f);
  _lfo.setAmplitude(lfo_a);
  
  Signal* signal_duty = _lfo.generate();
  _osc.setShape(duty);
  _osc.getShape().add(signal_duty);
  _osc.getShape().saturate(-0.99f, 0.99f);
  _osc.step(leftout);
  
  if (visualize)
  {
    _instrument->getParameter(PARAM_PURESQUARE_DUTY_ID)->setAuto(true,255.f*(_osc.getShape().samples[0]));
  }
  
  if (rightout)
    *rightout=*leftout;
}

PureSquare::PureSquare() :
Instrument<PureSquareVoice>(),
_lfo_amp(0,0,100),
_lfo_frequency(0,0,100),
_duty(127,0,255)
{

}

PureSquare::~PureSquare()
{
}

InstrumentParameter* PureSquare::getParameter(unsigned char id)
{
  switch (id)
  {
    case PARAM_PURESQUARE_LFOFREQUENCY_ID: return &_lfo_frequency;
    case PARAM_PURESQUARE_LFOAMP_ID: return &_lfo_amp;
    case PARAM_PURESQUARE_DUTY_ID: return &_duty;
    default: return Instrument<PureSquareVoice>::getParameter(id);
  }
}

PureSquareInterface::PureSquareInterface(PureSquare* instrument,const sf::Vector2f& size):
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

PureSquareInterface::~PureSquareInterface()
{
  if (_lfoAmpKnob) delete _lfoAmpKnob;
  if (_lfoFrequencyKnob) delete _lfoFrequencyKnob;
  if (_dutyKnob) delete _dutyKnob;
}