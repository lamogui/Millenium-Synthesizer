

#include "note.hpp"
#include "instrument.hpp"
 
Note::Note(unsigned int st, float f, float v):
_voiceUsed(0),
start(st),
lenght(0),
frequency(f),
velocity(v)
{
}

Note::Note(const Note& c) : 
_voiceUsed(0),
start(c.start),
lenght(c.lenght),
frequency(c.frequency),
velocity(c.velocity)
{

}

Note::~Note()
{
  sendStopSignal();
}

void Note::receivePlayedSignal(InstrumentVoice* v)
{
  sendStopSignal(); //be sure we don't have a voice which take care of us for security reasons
  _voiceUsed=v;
}
void Note::sendStopSignal()
{
  if (_voiceUsed)
  {
    _voiceUsed->endNote(); //tell to the instrument he can start release time !
    _voiceUsed=0;
  }
}

Note& Note::operator=(const Note& c)
{
  sendStopSignal();
  _voiceUsed = 0;
  start = c.start;
  lenght = c.lenght;
  frequency = c.frequency;
  velocity = c.velocity;
}

InstrumentParameter::InstrumentParameter(short value, short min, short max) :
_value(value),
_min(min),
_max(max)
{
}

InstrumentParameter::InstrumentParameter(const InstrumentParameter& p) :
_value(p._value),
_min(p._min),
_max(p._max)
{
}

short InstrumentParameter::operator++(int) { 
  _value++; 
  if (_value > _max) {_value=_min; return _max;} 
  return _value-1;
}

short InstrumentParameter::operator--(int){
  _value--; 
  if (_value < _min) {_value=_max; return _min;}
  return _value + 1;
}

InstrumentParameter& InstrumentParameter::operator=(const InstrumentParameter& p)
{
  _value = p._value;
  _min = p._min;
  _max = p._max;
  return *this;
}

bool InstrumentParameter::setValue(short v)
{
  if (v>=_min && v <= _max)
  {
    _value=v; return true;
  }
  return false;
}