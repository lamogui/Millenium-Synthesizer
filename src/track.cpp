/*******************************************************
Nom ......... : note.cpp
Role ........ : Implémente les classes permettant de 
                manipuler des données rythmiques en 
                lien avec un morceau et un instrument
Auteur ...... : Julien DE LOOR
Version ..... : V1.0 olol
Licence ..... : © Copydown™
********************************************************/

#include "track.hpp"
#include <iostream>
#include "bass.h"
#include <iostream>
#include <map>

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
   reset();
}

void Track::reset()
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
  _notes.clear();
  _events.clear();
  _time=0;
  _currentNote=0;
  _currentEvent=0;
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

bool Track::importFromMidiTrack(const Midi_track& midi)
{
   reset();
   unsigned int target_size=midi.chunk_size();
   unsigned g=0;
   const unsigned char* buffer=midi.get_chunk();
   std::map<BYTE, Note*> keyboard;
   
   unsigned int integ_delta;
   while (g < target_size)
   {
     //Read delta 
     BYTE delta_byte;
     unsigned int delta=0;
      do {
       delta_byte=buffer[g++];
       delta = (delta << 7) | (delta_byte & 0x7F);
     } while (delta_byte & 0x80 && g < target_size);
     //std::cout << "delta " << delta << std::endl;
     integ_delta+=delta;
     
     if (midi.get_head().need_bpm())
      _time=integ_delta/(midi.get_head().gain()*140.f); //TODO un getteur de BPM
     else
      _time=integ_delta/midi.get_head().gain();
     
     
     if (g >= target_size) break;
     
     //Read type
     BYTE type=buffer[g++];
     if (type==0xFF) { //Meta Event 
       if (g >= target_size) break;
       BYTE meta_type=buffer[g++];
       
       BYTE length_byte;
       unsigned int length=0;
       do {
         length_byte=buffer[g++];
         length = (length << 7) | (length_byte & 0x7F);
       } while (length_byte & 0x80 && g < target_size);
       //std::cout << "Jump meta " << (unsigned) meta_type << " len " << length << std::endl;
     
       switch (meta_type) {
       /*  case 0x01: //Text event
           for (unsigned int i=0;i<length && g < target_size;i++)
             _comment += buffer[g++];
           break;
         case 0x02: //Copyright
           for (unsigned int i=0;i<length && g < target_size;i++)
             _copyright += buffer[g++];
           break;
         case 0x03: //Sequence/Track Name
           for (unsigned int i=0;i<length && g < target_size;i++)
             _music_name += buffer[g++];
           break;*/
         case 0x2F: //End
           _time=0;
           return true;
         default:
           g+=length; //don't care
           break;
       }
       
     }
     else if ((type & 0xF0) == 0xF0) { //System event don't care
       BYTE length_byte;
       unsigned int length=0;
       do {
         length_byte=buffer[g++];
         length = (length << 7) | (length_byte & 0x7F);
       } while (length_byte & 0x80 && g < target_size);
       g+=length; //don't care
     }
     else  { //Midi event
       if (g >= target_size) break;
       BYTE p1,p2;
       //printf("Midi event 0x%X\n", type);
       BYTE midi_type=type>> 4;
       switch (midi_type) {
         case 0x9:
            p1=buffer[g++];
            p2=buffer[g++];
            if (20<p1<109 && keyboard.find(p1) == keyboard.end()) {
               std::cout << "add note " << (int) p1 << " time " << _time << std::endl;
               Note* note = new Note(_time, p1-20, (float)p2/(float)127.f);
               keyboard[p1]=note;
               _notes.push_back(note);
            }
            break;
         case 0x8:
            p1=buffer[g++];
            p2=buffer[g++];
            if (20<p1<109 && keyboard.find(p1) != keyboard.end()) {
               keyboard[p1]->length = _time-keyboard[p1]->start;
               keyboard.erase(p1);
            }
            break;
         case 0xC:
         case 0xD:
           //printf("Jump midi event 0x%X with 1\n",(unsigned int)midi_type);
           g++; //don't care
           break;
         default:
           //printf("Jump midi event 0x%X with 2\n",(unsigned int)midi_type);
           g+=2; //don't care
           break;
       }
     }
   }
   _time=0;
   std::cout << "Import midi : Wrong end" << std::endl;
   return true;
}

void Track::exportToMidiTrack(Midi_track& midi) const
{
  unsigned int currentNote=0;
  unsigned int  currentEvent=0;
  unsigned int  time=0;
  unsigned int  last_time=0;
  std::vector<Note*> played;
  const float gain = midi.get_head().gain();

  //Instrument grand piano
  
  for (;_notes.size() > currentNote || played.size(); time++)
  {
    //Add currently pressed notes !
    while (_notes.size() > currentNote && _notes[currentNote]->start == time)
    {
      played.push_back(_notes[currentNote]);
      float delta=(float)(time-last_time);
      delta *=gain;
      //std::cout << "Delta " << (DWORD)delta << std::endl;
      last_time=time;
      //Envoie message Note On !
      midi.push_midi_event((DWORD)delta, MIDI_NOTE_ON, 0, _notes[currentNote]->id + 21, _notes[currentNote]->velocity*127);
      currentNote++;
    }
    //Add current moved knob !
    while (_events.size() > currentEvent && _events[currentEvent]->appear == time)
    {
      float delta=(float)(time-last_time);
      delta *=gain;
      last_time=time;
      midi.push_midi_event((DWORD)delta, MIDI_CONTROLLER, 0, _events[currentEvent]->id  , _events[currentEvent]->value);
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
        //std::cout << "Delta " << (DWORD)delta << std::endl;
        //Envoie Message Note Off
        midi.push_midi_event((DWORD)delta, MIDI_NOTE_OFF, 0, played[i]->id + 21 , played[i]->velocity*127);
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

bool Track::concatene(const Track &track_extern) {
  std::vector<Note*> tempo;
  unsigned int g_this=0, g_extern=0, g_tempo=0;
  while (g_this<_notes.size() || g_extern<track_extern._notes.size()) {
    if (_notes[g_this]->start>track_extern._notes[g_extern]->start) {
      tempo.push_back(new Note(*track_extern._notes[g_extern++]));
    }
    else {
      tempo.push_back(_notes[g_this++]);
    }
  }
  while (g_this<_notes.size())
    tempo.push_back(_notes[g_this++]);
  while (g_extern<_notes.size())
    tempo.push_back(new Note(*track_extern._notes[g_extern++]));
  _notes.swap(tempo);
}

