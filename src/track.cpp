#include "track.hpp"
#include <iostream>
#include "bass.h"
#include <iostream>

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
  for (unsigned int i=0;i<_notes.size();i++)
  {
    delete _notes[i];
  }
  for (unsigned int i=0;i<_events.size();i++)
  {
    delete _events[i];
  }

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
    if (_notes[i]->start >= time) {
      _currentNote=i;
      found=true;
      _time=time;
      break;
    }
    else if (_notes[i]->start + _notes[i]->length >= time) {
      _currentNote=i;
      _time=_notes[i]->start;
      found=true;
    }
  }
  for (unsigned int i=0;i<_events.size();i++)
  {
    if (_events[i]->appear >= _time) {
      _currentEvent=i;
      break;
    }
  }
  if (!found) _time = time;
  return found;
}


unsigned int Track::fastLength()
{
  if (_notes.size())
    return _notes[_notes.size()-1]->start + _notes[_notes.size()-1]->length;
  else return 0;
}

void Track::exportToMidiTrack(Midi_track& midi) const
{
  unsigned currentNote=0;
  unsigned currentEvent=0;
  unsigned time=0;
  unsigned last_time=0;
  std::vector<Note*> played;
  
  const float gain = midi.get_head().gain();
  
  for (;_notes.size() > currentNote && played.size(); time++)
  {
    //Add currently pressed notes !
    while (_notes.size() > currentNote && _notes[currentNote]->start == time)
    {
      played.push_back(_notes[currentNote]);
      float delta=(float)(time-last_time);
      delta *=gain;
      last_time=time;
      //Envoie message Note On !
      midi.push_midi_event(delta, MIDI_NOTE_ON, 0, _notes[currentNote]->id + 21, _notes[currentNote]->velocity*127);
      currentNote++;
    }
    //Add current moved knob !
    while (_events.size() > currentEvent && _events[currentEvent]->appear == time)
    {
      float delta=(float)(time-last_time);
      delta *=gain;
      last_time=time;
      midi.push_midi_event(delta, MIDI_CONTROLLER, 0, _events[currentEvent]->id  , _events[currentEvent]->value);
      currentEvent++;
    }
    //remove current releases notes !
    for (int i=0;i<(int)played.size();i++)
    {
      if( played[i]->start + played[i]->length <= time)
      {
        float delta=(float)(time-last_time);
        delta *=gain;
        last_time=time;
        //Envoie Message Note Off
        midi.push_midi_event(delta, MIDI_NOTE_OFF, 0, played[i]->id + 21 , played[i]->velocity*127);
        played.erase(played.begin() + i);
        i--;
      }
    }
  }
}

bool Track::tick()
{
  //Add currently pressed notes !
  while (_notes.size() > _currentNote && _notes[_currentNote]->start == _time)
  {
    if (_instrument && _notes[_currentNote]->length) {
      _instrument->playNote(*_notes[_currentNote]);
      _played.push_back(_notes[_currentNote]);
    }
    _currentNote++;
  }
  //Add current moved knob !
  while (_events.size() > _currentEvent && _events[_currentEvent]->appear == _time)
  {
    if (_instrument) {
      _instrument->setParameterValue(_events[_currentEvent]->id,_events[_currentEvent]->value);
    }
    _currentEvent++;
  }
  //remove current releases notes !
  for (int i=0;(unsigned)i<_played.size();i++)
  {
    if(_played[i]->start + _played[i]->length <= _time)
    {
      _played[i]->sendStopSignal();
      _played.erase(_played.begin() + i);
      i--;
    }
  }
  //increment time
  _time++;
  if (_played.size() || _notes.size() > _currentNote) return false;
  return true;
}

Note* Track::recordNoteStart(unsigned char id, float v)
{
  if (_notes.size() > _currentNote)
  {
    _notes.insert(_notes.begin() + _currentNote,new Note(_time,id,v));
  }
  else {
    _notes.push_back(new Note(_time,id,v));
    _currentNote = _notes.size()-1;
  }
  return _notes[_currentNote];
}

void Track::recordNoteRelease(Note* n)
{
  if (n && _time > n->start)
    n->length=_time-n->start;
}