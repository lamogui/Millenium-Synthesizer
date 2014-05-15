/*****************************************************************************
Nom ......... : midi.hpp
Role ........ : Implemente les classes ayant une relation avec le format MIDI
                voir : http://www.sonicspot.com/guide/midifiles.html
Auteur ...... : Kwon-Young CHOI & Julien DE LOOR
Version ..... : V1.0 olol
Licence ..... : © Copydown™
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "midi.hpp"
#include <iostream>

//////////MIDI HEAD\\\\\\\\\\\\\\\\\\\\

Midi_head::Midi_head(WORD format, WORD tracks, WORD tick_per_beat) :
  _beat(false),
  _gain((float) tick_per_beat / (float) (Signal::refreshRate * 60.0 )),
  _format(format),
  _tracks(tracks),
  _division(tick_per_beat)
{
  if (!_gain) _gain = 1.f;
 }

Midi_head::Midi_head(WORD format, WORD tracks, BYTE frame, BYTE ticks) :
  _beat(true),
  _gain((float) frame*ticks /  (float) Signal::refreshRate ),
  _format(format),
  _tracks(tracks),
  _division(((frame | 0x80) << 8) | ticks)
{
  if (!_gain) _gain = 1.f;
 }

Midi_head::~Midi_head() {

}

void Midi_head::print_infos() const 
{
  printf("Midi_head 0x%X infos: \n",(unsigned int) this);
  printf("      format: %u \n", _format);
  printf("      tracks: %u \n", _tracks);
  printf("      division: 0x%X \n", _division);
  if (_beat)
  printf("      gain: %f (need bpm) \n", _gain);
  else 
  printf("      gain: %f (fps based) \n", _gain);
}

bool Midi_head::read_from_buffer(const unsigned char* buffer, unsigned int size )
{
  unsigned int g=0;
  if (!buffer) return false;
  if (size < Midi_head::size) return false;
  if (buffer[g++]!='M' || buffer[g++]!='T' || buffer[g++]!='h' || buffer[g++]!='d') return false;
  if (buffer[g++]!=0 || buffer[g++]!=0 || buffer[g++]!=0 || buffer[g++]!=6) return false;
  WORD format, tracks, division;
  format = buffer[g++] << 8;
  format |= buffer[g++];
  if (format !=1){
    printf("Midi_head error: Format %u unsupported yet\n",format); 
    return false;
  }
  
  tracks = buffer[g++] << 8;
  tracks |= buffer[g++];
  
  division = buffer[g++] << 8;
  division |= buffer[g++];
  
  if (division & 0x8000) {
    BYTE frame=(division >> 8) & 0x7F, ticks=(division & 0xFF);
    if (! frame*ticks)
    {
      printf("Midi_head error: invalid fps time division\n");
      return false;
    }
    _gain=(float) frame*ticks / (float) Signal::refreshRate;
    _beat=false;
  }
  else {
    if (!division){
      printf("Midi_head error: invalid beat time division\n");
      return false;
    }
    _gain=(float) division / (float) (Signal::refreshRate * 60.f);
    _beat=true;
  }
  
  _tracks=tracks;
  _division=division;
  
  return true;
}

bool Midi_head::write_to_buffer(unsigned char* buffer, unsigned int size ) const {
  if (!buffer) return false;
  if (size < Midi_head::size) return false;

  //magic
  *buffer++='M';
  *buffer++='T';
  *buffer++='h';
  *buffer++='d';
  *buffer++=0;
  *buffer++=0;
  *buffer++=0;
  *buffer++=6;

  if (_format!=1) {
    printf("Midi_head error: Format %u unsupported yet\n",_format); 
    return false;
  }
  else {
    *buffer++=0;
    *buffer++=1;
  }
  //nombre de piste
  *buffer++=_tracks>>8;
  *buffer++=_tracks & 0xFF;

  //resolution
  *buffer++=_division>>8;
  *buffer++=_division & 0xFF;
  /*
  *buffer++=0x01;
  *buffer++=0xe0;
  */
  return true;
}

bool Midi_head::write_to_file(FILE* file) const
{
  unsigned char buffer[Midi_head::size];
  if (write_to_buffer(buffer,Midi_head::size))
  {
    if (Midi_head::size==fwrite((void*)buffer,1,Midi_head::size,file))
    {
      return true;
    }
    printf("Midi_head error: fwrite error !\n"); 
  }
  return false;
}

//////////MIDI TRACK0\\\\\\\\\\\\\\\

Midi_track0::Midi_track0() :
_mpqn(0),
_music_name(""),
_copyright("Millenium Synthesizer"),
_comment("May the force be with you")
{
}

Midi_track0::Midi_track0(const std::string n) :
_mpqn(0),
_music_name(n),
_copyright("Millenium Synthesizer"),
_comment("May the force be with you")
{
}

Midi_track0::~Midi_track0()
{
}

unsigned int Midi_track0::size() const
{
  // Magic + Head + Delta + MetaEvent + EndOfTrack + MetaSize
  unsigned int size = 4 + 4 + 1 + 1 + 1 + 1; //minimum size
  //                   Delta + MetaEvent + Type + length + Specific size
  if (_music_name.size()) size += 1 + 1 + 1 + 1 + _music_name.size();
  if (_copyright.size()) size += 1 + 1 + 1 + 1 + _copyright.size();
  if (_comment.size()) size += 1 + 1 + 1 + 1 + _comment.size();
  if (_mpqn) size += 1 + 1 + 1 + 1 + 3;
  return size;
}

unsigned int Midi_track0::read_from_buffer(const unsigned char* buffer, unsigned int s)
{
  reset();
  unsigned int min_size = size(); 
  if (s > min_size)
  {
    unsigned int g=0;
    if (buffer[g++]!='M' || buffer[g++]!='T' || buffer[g++]!='r' || buffer[g++]!='k') return 0;
    unsigned int target_size=0;
    target_size |= buffer[g++] << 24;
    target_size |= buffer[g++] << 16;
    target_size |= buffer[g++] << 8;
    target_size |= buffer[g++];
    target_size+=g;
    if (s > target_size)
    {
      while (g < target_size)
      {
        //Read delta (and don't care)
        BYTE delta_byte;
        unsigned int delta=0;
         do {
          delta_byte=buffer[g++];
          delta = (delta << 7) | (delta_byte & 0x7F);
        } while (delta_byte & 0x80 && g < target_size);
        
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
        
          switch (meta_type) {
            case 0x01: //Text event
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
              break;
            case 0x2F: //End
              return target_size;
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
          switch (type >> 4) {
            case 0xC:
            case 0xD:
              g++; //don't care
              break;
            default:
              g+=2; //don't care
              break;
          }
        }
      }
      return target_size;
    }
    else 
    {
      return 0;
    }
  }
  return 0;
}


void Midi_track0::print_infos() const {
  printf("Midi_track0 0x%X infos: \n",(unsigned int) this);
  printf("      name: %s \n", _music_name.c_str());
  printf("      copyright: %s \n", _copyright.c_str());
  printf("      comment: %s \n", _comment.c_str());
}

bool Midi_track0::write_to_buffer(unsigned char* buffer, unsigned int s) const
{
  if (!buffer) return false;
  unsigned int target_size=size();
  unsigned char str_length;
  if (s < target_size) return false;
  
  //Magic
  *buffer++='M';
  *buffer++='T';
  *buffer++='r';
  *buffer++='k';
  
  //Head
  *buffer++=(target_size-8)>>24;
  *buffer++=((target_size-8)>>16) & 0xFF;
  *buffer++=((target_size-8)>>8) & 0xFF;
  *buffer++=(target_size-8) & 0xFF;
  
  if (_music_name.size()) {
    //Meta Event track name
    *buffer++=0;    //Delta
    *buffer++=0xFF; //Meta
    *buffer++=0x3;  //Track Name 
    str_length=_music_name.size() & 0x7F; //Beurk here 
    *buffer++=str_length;
    for (unsigned char i=0; i<str_length; i++) 
      *buffer ++= (unsigned char) _music_name[i];
  }
  
  if (_copyright.size()) {
    *buffer++=0;    //Delta
    *buffer++=0xFF; //Meta
    *buffer++=0x2; //Copyright 
    str_length=_copyright.size() & 0x7F; //Beurk here 
    *buffer++=str_length;
    for (unsigned char i=0; i<str_length; i++) 
      *buffer ++= (unsigned char) _copyright[i];
  }
  
  if (_comment.size()) {
    *buffer++=0;    //Delta
    *buffer++=0xFF; //Meta
    *buffer++=0x1;  //Text Event 
    str_length=_comment.size() & 0x7F; //Beurk here 
    *buffer++=str_length;
    for (unsigned char i=0; i<str_length; i++) 
      *buffer ++= (unsigned char) _comment[i];
  }
  
  if (_mpqn) {
    *buffer++=0;     //Delta
    *buffer++=0xFF;  //Meta
    *buffer++=0x51;  //Set Tempo
    *buffer++=3;
    *buffer++=_mpqn>>16;
    *buffer++=(_mpqn>>8) & 0xFF;
    *buffer++=_mpqn&0xFF;
  }
  
  //End of the Track !
  *buffer++=0;
  *buffer++=MIDI_META;
  *buffer++=MIDI_END_OF_TRACK;
  *buffer++=0; //Size used next to the event declaration
  
  return true;
}

bool Midi_track0::write_to_file(FILE* file) const
{
  unsigned int target_size=size();
  unsigned char* buffer = (unsigned char*) malloc(target_size);
  if (write_to_buffer(buffer,target_size))
  {
    if (target_size==fwrite((void*)buffer,1,target_size,file))
    {
      free(buffer);
      return true;
    }
    printf("Midi_track0 error: fwrite error !\n"); 
  }
  else 
  {
    printf("error !\n"); 
  }
  free((void*)buffer);
  return false;
}

//////////MIDI TRACK\\\\\\\\\\\\\\\\\\\\

Midi_track::Midi_track(Midi_head& head) :
_head(&head),
_chunk(0),
_alloc_size(256),
_chunk_size(0)
{
  _chunk=(unsigned char*)malloc(_alloc_size);
}

Midi_track::~Midi_track() {
  free((void*)_chunk);
}

unsigned int Midi_track::size() const
{
  // Magic + ChunkSize + _chunk_size + Delta + MetaEvent + EndOfTrack + MetaSize
  return 4 + 4 + _chunk_size + 1 + 1 + 1 + 1;
}

bool Midi_track::write_to_buffer(unsigned char* buffer, unsigned int s ) const
{
  unsigned int target_size=size();
  if (s < target_size) return false;
  
  //Magic
  *buffer++='M';
  *buffer++='T';
  *buffer++='r';
  *buffer++='k';
  
  //Head
  *buffer++=(target_size-8)>>24;
  *buffer++=((target_size-8)>>16) & 0xFF;
  *buffer++=((target_size-8)>>8) & 0xFF;
  *buffer++=(target_size-8) & 0xFF;
  
  //Copy Custom Datas
  memcpy((void*) buffer,(const void*)_chunk,_chunk_size);
  buffer+=_chunk_size; //Move to end of chunk
  
  
  //Finally the end of the track
  *buffer++=0;
  *buffer++=MIDI_META;
  *buffer++=MIDI_END_OF_TRACK;
  *buffer++=0; //Size used next to the event declaration
  
  return true;
}

bool Midi_track::write_to_file(FILE* file) const
{
  unsigned int target_size=size();
  unsigned char* buffer = (unsigned char*) malloc(target_size);
  if (write_to_buffer(buffer,target_size))
  {
    if (target_size==fwrite((void*)buffer,1,target_size,file))
    {
      free(buffer);
      return true;
    }
    printf("Midi_track error: fwrite error !\n"); 
  }
  free((void*)buffer);
  return false;
}


unsigned int Midi_track::read_from_buffer(const unsigned char* buffer, unsigned int s)
{
  reset();
  unsigned int min_size = size(); 
  if (s >= min_size && buffer)
  {
    unsigned int g=0;
    if (buffer[g++]!='M' || buffer[g++]!='T' || buffer[g++]!='r' || buffer[g++]!='k') {
      printf("Midi_track error: wrong magic !\n"); 
      return 0;
    }
    unsigned int target_size=0;
    target_size |= buffer[g++] << 24;
    target_size |= buffer[g++] << 16;
    target_size |= buffer[g++] << 8;
    target_size |= buffer[g++];
    if (s >= target_size + g && target_size >= 4)
    {
      check_alloc(target_size);
      memcpy((void*)_chunk,(const void*) &(buffer[g]),target_size);
      _chunk_size = target_size;
      if (_chunk[_chunk_size-3] == 0xFF && 
          _chunk[_chunk_size-2] == 0x2F && 
          _chunk[_chunk_size-1] == 0x00) //detect End Of Track Meta Event
      {
        _chunk_size-=4;
      }
      return target_size + g;
    }
    else 
    {
      printf("Midi_track error: missing needed data require %u bytes but have %u\n",target_size+g,s); 
    }
  }
  return 0;
}


void Midi_track::push_varlength(DWORD var) {
  DWORD var_tp=var;
  DWORD bit=0;
  BYTE part;
  check_alloc(4); //Maximum 4 bytes length
  
  //32 bits unsigned integer
  
  //Who is the most significant bit ?
  for (DWORD i=0; i<32; i++) {
    if (var_tp&1) bit=i;
    var_tp=var_tp>>1;
  }
  part=bit/7;
  
  for (DWORD i=0; i<(unsigned)part+1; i++) {
    if (i==part) {
      _chunk[_chunk_size++] = var & 0x7F;
    }
    else {
      _chunk[_chunk_size++] = ((var>>(7*(part-i))) & 0x7F) | 0x80;
    }
  }
}

void Midi_track::push_midi_event(DWORD midi_delta, BYTE type, BYTE chan, BYTE p1, BYTE p2)
{
  push_varlength(midi_delta);
  
  check_alloc(3);
  _chunk[_chunk_size++] = (type & 0xF) << 4 | (chan & 0xF);
  _chunk[_chunk_size++] = p1 & 0x7F;
  _chunk[_chunk_size++] = p2 & 0x7F;
}

void Midi_track::push_midi_event(DWORD midi_delta, BYTE type, BYTE chan, BYTE p1)
{
  push_varlength(midi_delta);
  check_alloc(2);
  _chunk[_chunk_size++] = (type & 0xF) << 4 | (chan & 0xF);
  _chunk[_chunk_size++] = p1 & 0x7F;
}

