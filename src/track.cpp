
/****************************************************************************
Nom ......... : track.cpp
Role ........ : Definie une piste (partition de musique)
Auteur ...... : Julien DE LOOR & Kwon-Young CHOI
Version ..... : V1.7 olol
Licence ..... : © Copydown™
****************************************************************************/

#ifdef _MSC_VER
#pragma warning( disable : 4244 )
#pragma warning( disable : 4804 )
#pragma warning( disable : 4267 )
#pragma warning( disable : 4996 )
#endif

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
  sf::Lock lock(_mutex);
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
  sf::Lock lock(_mutex);
  for (unsigned int i=0;i<_played.size();i++)
  {
    _played[i]->sendStopSignal();
  }
  _played.clear();
}

bool Track::seek(unsigned int time)
{
  panic();
  sf::Lock lock(_mutex);
  _currentNote=0;
  _currentEvent=0;
  bool found = false;
  for (unsigned int i=0;i<_notes.size();i++)
  {
    if (_notes[i]->start() >= time) {
      _currentNote=i;
      found=true;
      _time=time;
      break;
    }
    else if (_notes[i]->start() + _notes[i]->length() >= time) {
      _currentNote=i;
      _time=_notes[i]->start();
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
  sf::Lock lock(_mutex);
  if (_notes.size())
    return _notes[_notes.size()-1]->start() + _notes[_notes.size()-1]->length();
  else return 0;
}

bool Track::importFromMidiTrack(const Midi_track& midi)
{
   reset();
   sf::Lock lock(_mutex);
   unsigned int target_size=midi.chunk_size();
   unsigned g=0;
   const unsigned char* buffer=midi.get_chunk();
   std::map<BYTE, Note*> keyboard;
    
   unsigned int integ_delta=0;
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
               //std::cout << "add note " << (int) p1 << " time " << _time << std::endl;
               Note* note = new Note(_time, p1-21, (float)p2/(float)127.f);
               keyboard[p1]=note;
               _notes.push_back(note);
            }
            break;
         case 0x8:
            p1=buffer[g++];
            p2=buffer[g++];
            if (20<p1<109 && keyboard.find(p1) != keyboard.end()) {
               keyboard[p1]->setLength(_time-keyboard[p1]->start());
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
  sf::Lock lock(_mutex);
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
    while (_notes.size() > currentNote && _notes[currentNote]->start() == time)
    {
      played.push_back(_notes[currentNote]);
      float delta=(float)(time-last_time);
      delta *=gain;
      //std::cout << "Delta " << (DWORD)delta << std::endl;
      last_time=time;
      //Envoie message Note On !
      midi.push_midi_event((DWORD)delta, MIDI_NOTE_ON, 0, _notes[currentNote]->id() + 21, _notes[currentNote]->velocity()*127);
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
      if( played[i]->start() + played[i]->length() <= time)
      {
        float delta=(float)(time-last_time);
        delta *=gain;
        last_time=time;
        //std::cout << "Delta " << (DWORD)delta << std::endl;
        //Envoie Message Note Off
        midi.push_midi_event((DWORD)delta, MIDI_NOTE_OFF, 0, played[i]->id() + 21 , played[i]->velocity()*127);
        played.erase(played.begin() + i);
        i--;
      }
    }
  }
}

bool Track::tick()
{
  sf::Lock lock(_mutex);
  //Add currently pressed notes !
  while (_notes.size() > _currentNote && _notes[_currentNote]->start() == _time)
  {
    if (_instrument && _notes[_currentNote]->length()) {
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
    if(_played[i]->start() + _played[i]->length() <= _time)
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
  sf::Lock lock(_mutex);
  if (n && _time > n->start())
    n->setLength(_time-n->start());
}

void Track::concatenate(const Track &track_extern) {
  sf::Lock lock(_mutex);
  sf::Lock extlock(track_extern._mutex);
  std::vector<Note*> tempo;
  unsigned int g_this=0, g_extern=0;
  while (g_this<_notes.size() && g_extern<track_extern._notes.size()) {
    if (this->_notes[g_this]->start() > track_extern._notes[g_extern]->start()) {
      tempo.push_back(new Note(*track_extern._notes[g_extern++]));
    }
    else {
      tempo.push_back(this->_notes[g_this++]);
    }
  }
  while (g_this<_notes.size())
    tempo.push_back(this->_notes[g_this++]);
  while (g_extern< track_extern._notes.size())
    tempo.push_back(new Note(*track_extern._notes[g_extern++]));
  _notes.swap(tempo);
}


void SaveTrackToMIDIFileRoutine(const Track* t)
{
  char filename[0x104]=""; //Maxpath
  #ifdef COMPILE_WINDOWS
  char path[0x104]={0};
  GetCurrentDirectoryA(0x104, path);
  OPENFILENAMEA ofn;
  memset(&ofn, 0, sizeof(ofn) );
  ofn.lStructSize=sizeof(OPENFILENAME);
  ofn.lpstrFilter="MIDI Files\0*.mid;*.midi\0\0";
  ofn.nMaxFile=0x104;
  ofn.lpstrFile=filename;
  ofn.lpstrTitle="Save to midi";
  ofn.Flags=OFN_HIDEREADONLY|OFN_EXPLORER;
  if (!GetSaveFileNameA(&ofn)) return;
  SetCurrentDirectoryA(path);
  #else
  std::string input;
  std::cout << "Please specify MIDI output filename: " << std::endl;
  std::cin.clear();
  getline(std::cin, input);
  if (input.empty()) return;
  strncpy(filename,input.c_str(),0x103);
  #endif
   
  Midi_head head(1,2,25,2);
  Midi_track0 track0;
  Midi_track track(head);
  t->exportToMidiTrack(track);
   
  FILE* file=fopen(filename,"wb");
  if (!file) {
    std::cout << "Unable to open file " << filename << std::endl;
    return;
  }
  head.write_to_file(file);
  track0.write_to_file(file);
  track.write_to_file(file);
  fclose(file);
  
  
  std::cout << "File size : " << head.size + track0.size() + track.size() << "\n";
}

void OpenFromMIDIFileRoutine(Track* t)
{
  if (!t) {
    std::cerr << "No track to load" << std::endl;
    return;
  }
  char filename[0x104]=""; //Maxpath
  
  #ifdef COMPILE_WINDOWS
  char path[0x104]={0};
  GetCurrentDirectoryA(0x104, path);
  
  OPENFILENAMEA ofn;
  memset(&ofn, 0, sizeof(ofn) );
  ofn.lStructSize=sizeof(OPENFILENAME);
  ofn.lpstrFilter="MIDI Files\0*.mid;*.midi\0\0";
  ofn.nMaxFile=0x104;
  ofn.lpstrFile=filename;
  ofn.lpstrTitle="Load from midi";
  ofn.Flags=OFN_HIDEREADONLY|OFN_EXPLORER;
  if (!GetOpenFileNameA(&ofn)) return;
  SetCurrentDirectoryA(path);
  #else
  std::string input;
  std::cout << "Please specify MIDI input filename: " << std::endl;
  std::cin.clear();
  getline(std::cin, input);
  if (input.empty()) return;
  strncpy(filename,input.c_str(),0x103);
  #endif
  
  FILE* file = fopen(filename, "rb");
  if (file)
  {
    t->reset();
    int filesize = fsize(file);
    unsigned char* buffer= (unsigned char*) malloc(filesize);
    fread(buffer,1,filesize,file);
    Midi_head head(1, 0, 25, 2);
    if (head.read_from_buffer(buffer, filesize))
    {
      std::cout << "File " << filename << " infos" << std::endl;
      std::cout << "Header (" << Midi_head::size << " bytes):" << std::endl;
      head.print_infos();
      if (head.format() == 1)
      {        
        Midi_track0 track0;
        unsigned int track0_len;
        if (track0_len = track0.read_from_buffer(buffer+Midi_head::size, filesize-Midi_head::size))
        {
          std::cout << "Track 0 (" << track0_len << " bytes):"<< std::endl;
          track0.print_infos();
          unsigned int delta = Midi_head::size + track0_len;
          Midi_track track(head);
          Track tempTrack;
          unsigned int track_len;
          unsigned count=0;
          while (filesize > (int) delta)
          {
            if (track_len=track.read_from_buffer(buffer+delta, filesize-delta))
            {
              tempTrack.importFromMidiTrack(track);
              t->concatenate(tempTrack); 
              delta+=track_len;
              std::cout << "Track " << count + 1 <<  " (" << track_len << " bytes) Time " << tempTrack.fastLength() << " concatenate time " << t->fastLength() << std::endl;
              count++;
            }
            else {
              std::cout << "Failed to read Track " << count + 1 << std::endl;
              break;
            }
          }
          
          std::cout << "Readed " << count + 1 << "/" << head.tracks() << "  with success !" << std::endl;
          std::cout << "Duree  " << t->fastLength() << std::endl;
          
        }
        else 
        {
          std::cout << "Error: " << filename << " failed to load track 0" << std::endl;
        } 
      }
      else if (head.format() == 0)
      {
         Midi_track track(head);
         unsigned int track_len;
         if (track_len = track.read_from_buffer(buffer+Midi_head::size, filesize-Midi_head::size))
         {
           t->importFromMidiTrack(track);
           std::cout << "Track 0 (" << track_len << " bytes) Time " << t->fastLength() << std::endl;
         }
         else std::cout << "Failed to read MIDI 0 " << std::endl;
      }
      
    }
    else
    {
      std::cout << "Error: " << filename << " is not a compatible midi file" << std::endl; 
    }
    fclose(file);
  }
  else {
    std::cout << "Unable to open: " << filename << std::endl; 
  }
  
}

void RewindTrackRoutine(Track* t)
{
  if (t) t->seek(0);
}
