#include "cado.hpp"
#include <cmath>

CadoVoice::CadoVoice(AbstractInstrument* creator) :
InstrumentVoice(creator),
_osc1(new SquareOscillator),
_currentNote(0,NOT_A_NOTE)
{
  
}

CadoVoice::~CadoVoice()
{
  delete _osc1;
}

void CadoVoice::beginNote(Note& n)
{
  _currentNote = n;
  _osc1->resetTime();
  
  _osc1->setFrequency(_currentNote.frequency());
  _osc1->setAmplitude(_currentNote.velocity);
  
  _used=true;
}

void CadoVoice::endNote()
{
  _used=false;
}

void CadoVoice::step(Signal* leftout, Signal* rightout)
{
  
  _osc1->step(leftout);
  
  
  if (rightout)
    *rightout = *leftout;

}

Cado::Cado() :
Instrument<CadoVoice>()
{
  
}
 
Cado::~Cado()
{
}


void Cado::step(Signal* l, Signal* r)
{
  Instrument<CadoVoice>::step(l,r);
}

CadoInterface::CadoInterface(Cado* instrument, const sf::Vector2f& size):
Interface(sf::Vector2i(600,360),size),
_texture(),
_back(),
_instrument(instrument)
{
  if (_instrument && _texture.loadFromFile("img/Cado.png"))
  {
    _back.setTexture(_texture);
    _back.setTextureRect(sf::IntRect(0,0,600,360));

    addDrawable(&_back);
  }

}

CadoInterface::~CadoInterface()
{
}

