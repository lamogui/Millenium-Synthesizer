#include "track.hpp"
#include <iostream>
#include "bass.h"

Track::Track() :
_instrument(0),
_time(0),
_currentNote(0),
_currentEvent(0),
_events(),
_notes(),
_played()
{
}

Track::Track(AbstractInstrument* i) :
_instrument(i),
_time(0),
_currentNote(0),
_currentEvent(0),
_events(),
_notes(),
_played()
{
}

Track::~Track()
{
  panic();
}

void Track::panic()
{
  for (unsigned int i=0;i<_played.size();i++)
  {
    _played[i]->sendStopSignal();
  }
  _played.clear();
}

bool Track::seek(unsigned int time)
{
  panic();
  _currentNote=0;
  _currentEvent=0;
  bool found = false;
  for (unsigned int i=0;i<_notes.size();i++)
  {
    if (_notes.start >= time) {
      _currentNote=i;
      found=true;
      _time=time;
      break;
    }
    else if (_notes.start + _notes.lenght >= time) {
      _currentNote=i;
      _time=_notes.start
      found=true;
    }
  }
  for (unsigned int i=0;i<_events.size();i++)
  {
    if (_events.appear >= _time) {
      _currentEvent=i;
      break;
    }
  }
  return found;
}

bool Track::tick()
{
  //Add currently pressed notes !
  while (_notes.size() > _currentNote && _notes[_currentNote].start == _time)
  {
    if (_instrument) {
      _instrument->playNote(_notes[_currentNote]);
      _played.push_back(&_notes[_currentNote]);
    }
    _currentNote++;
  }
  //Add current moved knob !
  while (_events.size() > _currentEvent && _events[_currentEvent].appear == _time)
  {
    if (_instrument) {
      _instrument->setParameterValue(_events[_currentEvent].id,_events[_currentEvent].value);
    }
    _currentEvent++;
  }
  //remove current releases notes !
  for (unsigned int i=0;i<_played.size();i++)
  {
    if( _played[i]->start + _played[i]->lenght <= _time)
      _played[i]->sendStopSignal();
  }
  //increment time
  _time++;
  if (_played.size() || _notes.size() > _currentNote) return false;
  return true;
}

bool Track::loadFromMidi(std::ifstream& file, WORD time_division)
{
  panic();
  
  unsigned int last_event=0;
  float multiplier=1.f;
  float diviser=1.f;
  if (time_division & 0x8000)
  {
    multiplier = Signal::rate * 60.f / (float) (time_division & 0x7FFF);
  }
  else
  {
    unsigned fps = (time_division & 0x7F00) >> 8;
    float fps_f = (float) fps;
    if (fps == 29) fps_f=29.97f;
    multiplier = (float) Signal::rate  / (fps_f * (float) (time_division & 0xFF));
    diviser = 120.f; //default tempo
  }
  
  if (!file.is_open()) return false;
  
  bool errorfile=false;
  char magic[5] = {0};
  if (file.read(magic,4) && std::string("MTrk") == magic) {
    DWORD size=0;
    if (!file>>size) errorfile=true;
    std::cout << "Track chunk size: " << size <<std::endl;
    
    
  
    
  } 
  else {
    std::cerr << "Track load from MIDI error: Wrong magic" <<std::endl;
    return false;
  }
  
  
  
  
  seek(0);
}