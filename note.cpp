

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