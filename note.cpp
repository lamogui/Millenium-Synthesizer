

#include "note.hpp"
#include "instrument.hpp"
 
float Note::noteFrequency[256]={
27.5,29.1353,30.8677,32.7032,34.6479,36.7081,38.8909,41.2035,43.6536,46.2493,48.9995,51.9130,
55,  58.2705,61.7354,65.4064,69.2957,73.4162,77.7817,82.4069,87.3071,92.4986,97.9989,103.826,
110, 116.541,123.471,130.813,138.591,146.832,155.563,164.814,174.614,184.997,195.998,207.652,
220, 233.082,246.942,261.626,277.183,293.665,311.127,329.628,349.228,369.994,391.995,415.305,
440, 446.164,493.883,523.251,554.365,587.33, 622.254,659.255,698.456,739.989,783.991,830.609,
880, 932.328,987.767,1046.5, 1108.73,1174.66,1244.51,1318.51,1396.91,1479.98,1567.98,1661.22,
1760,1864.66,1975.53,2093,2217.46,2349.32,2489.02,2637.02,2793.02,2959.96,3135.96,3322.44,
3520,3729.31,3951.07,4186.01 };
 
Note::Note(unsigned int st, unsigned char i, float v):
_voiceUsed(0),
start(st),
lenght(0),
velocity(v),
id(i)
{
}

Note::Note(const Note& c) : 
_voiceUsed(0),
start(c.start),
lenght(c.lenght),
velocity(c.velocity),
id(c.id)
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
  id = c.id;
  velocity = c.velocity;
}

InstrumentParameter::InstrumentParameter(short value, short min, short max) :
_modified(true),
_value(value),
_min(min),
_max(max)
{
  notify();
}

InstrumentParameter::InstrumentParameter(const InstrumentParameter& p) :
_modified(true),
_value(p._value),
_min(p._min),
_max(p._max)
{
  notify();
}

short InstrumentParameter::operator++(int) { 
  _value++;

  if (_value > _max) {_value=_min; notify(); return _max;}
  else notify();
  return _value-1;
}

short InstrumentParameter::operator--(int){
  _value--; 
  
  if (_value < _min) {_value=_max; notify(); return _min;}
  else notify();
  return _value + 1;
}

InstrumentParameter& InstrumentParameter::operator=(const InstrumentParameter& p)
{
  _value = p._value;
  _min = p._min;
  _max = p._max;
  notify();
  return *this;
}

bool InstrumentParameter::setValue(short v)
{
  if (v>=_min && v <= _max)
  {
    _value=v; 
    notify();
    return true;
  }
  return false;
}