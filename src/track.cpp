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
  return found;
}


unsigned int Track::fastLength()
{
  if (_notes.size())
    return _notes[_notes.size()-1]->start + _notes[_notes.size()-1]->length;
  else return 0;
}

bool Track::saveToMidi(Midi_track *piste)
{
  char* midFile = piste->get_midFile();
  DWORD chunk_size = piste->get_chunk_size();
  DWORD size=0;
  //headers
  if (size>=chunk_size){ 
    midFile=(char*)realloc(midFile, chunk_size+100);
    chunk_size+=100;
    piste->set_midFile(midFile);
  }
  *midFile++='M';
  *midFile++='T';
  *midFile++='r';
  *midFile++='k';
  size+=4;

  //init 
  //chunk size
  if (size>=chunk_size){ 
    midFile=(char*)realloc(midFile, chunk_size+100);
    chunk_size+=100;
    piste->set_midFile(midFile);
  }
  *midFile++=0;
  *midFile++=0;
  *midFile++=0;
  char *p_size=midFile;
  *midFile++=size;
  size+=4;

  //meta event track name
  std::string name=piste->get_track_name();
  if (size>=chunk_size){ 
    midFile=(char*)realloc(midFile, chunk_size+100);
    chunk_size+=100;
    piste->set_midFile(midFile);
  }
  *midFile++=0;
  *midFile++=MIDI_META;
  *midFile++=MIDI_TRACK_NAME;
  *midFile++=(char)name.size();
  size+=4;
  for (unsigned int i=0; i<name.size(); i++) {
    *midFile++=(char)name[i];
    size++;
  }

  //event programme change event
  if (size>=chunk_size){ 
    midFile=(char*)realloc(midFile, chunk_size+100);
    chunk_size+=100;
    piste->set_midFile(midFile);
  }
  *midFile++=0;
  *midFile++=MIDI_PROGRAM_CHANGE;
  *midFile++=1;
  size+=3;

  //controller main volume event
  if (size>=chunk_size){ 
    midFile=(char*)realloc(midFile, chunk_size+100);
    chunk_size+=100;
    piste->set_midFile(midFile);
  }
  *midFile++=0;
  *midFile++=MIDI_CONTROLLER;
  *midFile++=MIDI_MAIN_VOLUME;
  *midFile++=100;
  size+=4;

  unsigned currentNote=0;
  unsigned currentEvent=0;
  unsigned time=0;
  unsigned last_time=0;
  std::vector<Note*> played;
  for (;_notes.size() > currentNote && played.size(); time++)
  {
    //Add currently pressed notes !
    while (_notes.size() > currentNote && _notes[currentNote]->start == time)
    {
      played.push_back(_notes[currentNote]);
      unsigned delta=time-last_time;
      last_time=time;

      //Envoie message Note On !
      //event note on
      piste->write_var(1000, midFile);
      *midFile++=MIDI_NOTE_ON;
      *midFile++=_notes[currentNote]->id+21;
      *midFile++=100;
      size+=5;
      currentNote++;
    }
    //Add current moved knob !
    while (_events.size() > currentEvent && _events[currentEvent]->appear == time)
    {
      currentEvent++;
    }
    //remove current releases notes !
    for (unsigned int i=0;i<played.size();i++)
    {
      if( played[i]->start + played[i]->length <= time)
      {
        unsigned delta=time-last_time;
        last_time=time;
        //Envoie Message Note Off
        played.erase(played.begin() + i);
        i--;
      }
    }
  }
  return 1;
}

bool Track::tick()
{
  //Add currently pressed notes !
  while (_notes.size() > _currentNote && _notes[_currentNote]->start == _time)
  {
    if (_instrument) {
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
  for (unsigned int i=0;i<_played.size();i++)
  {
    //Faille ici ! _played[i]->length peut-être = à 0 et ne JAMAIS Finir !
    if(_played[i]->length && _played[i]->start + _played[i]->length <= _time)
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
  if (n)
    n->length=_time-n->start;
}


bool Track::loadFromMidi(std::ifstream& file, WORD time_division, float &bpm)
{
  panic(); return false;
  /*
     unsigned int last_event=0;
     float multiplier=1.f;
  //float bpm=1.f;
  if (time_division & 0x8000)
  {
  multiplier = Signal::frequency * 60.f / (float) (time_division & 0x7FFF);
  //bpm = 120.f; //default tempo
  }
  else
  {
  unsigned fps = (time_division & 0x7F00) >> 8;
  float fps_f = (float) fps;
  if (fps == 29) fps_f=29.97f;
  multiplier = (float) Signal::frequency  / (fps_f * (float) (time_division & 0xFF));

  }


  if (!file.is_open()) return false;

  bool errorfile=false;
  char magic[5] = {0};
  if (file.read(magic,4) && strcmp("MTrk",magic) == 0) {
  DWORD size=0;
  if (!file>>size) errorfile=true;
  std::cout << "Track chunk size: " << size <<std::endl;




  } 
  else {
  std::cerr << "Track load from MIDI error: Wrong magic" <<std::endl;
  return false;
  }




  seek(0);*/
}
