#include <stdio.h>
#include <stdlib.h>
#include "midi.hpp"

Midi_head::Midi_head(WORD format, WORD tracks, BYTE frame, BYTE ticks) :
  _format(format),
  _tracks(tracks),
  _frame(frame),
  _ticks(ticks)
{

}

Midi_head::~Midi_head() {

}

bool Midi_head::write_to_buffer(unsigned char* buffer, unsigned int size ) {

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
  *buffer++=0x80|_frame;
  *buffer++=_ticks;
  return true;
}

bool Midi_head::write_to_file(FILE* file)
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

Midi_track::Midi_track(const std::string &track_name, DWORD tempo) :
  _midFile(NULL),
  _track_name(track_name),
  _tempo(tempo),
  _chunk_size(100)
{
  _midFile=(char*)malloc(100);
}

Midi_track::~Midi_track() {
  free(_midFile);
}

char * Midi_track::write_track0() {
  unsigned char size=0;
  char *head_midFile=_midFile;
  for (unsigned int i=0; i<_chunk_size; i++) {
    head_midFile[i]=0x0;
  }

  //chunk ID
  if (size>=_chunk_size){ _midFile=(char*)realloc(_midFile, _chunk_size+100);_chunk_size+=100;}
  *_midFile++='M';
  *_midFile++='T';
  *_midFile++='r';
  *_midFile++='k';
  size+=4;

  //chunk size
  if (size>=_chunk_size){ _midFile=(char*)realloc(_midFile, _chunk_size+100);_chunk_size+=100;}
  *_midFile++=0;
  *_midFile++=0;
  *_midFile++=0;
  char *p_size=_midFile;
  *_midFile++=size;
  size+=4;
  
  //meta event track name
  if (size>=_chunk_size){ _midFile=(char*)realloc(_midFile, _chunk_size+100);_chunk_size+=100;}
  *_midFile++=0;
  *_midFile++=MIDI_META;
  *_midFile++=MIDI_TRACK_NAME;
  *_midFile++=(char)_track_name.size();
  size+=4;
  for (unsigned int i=0; i<_track_name.size(); i++) {
    *_midFile++=(char)_track_name[i];
    size++;
  }

  //meta event set tempo
  if (size>=_chunk_size){ _midFile=(char*)realloc(_midFile, _chunk_size+100);_chunk_size+=100;}
  *_midFile++=0;
  *_midFile++=MIDI_META;
  *_midFile++=MIDI_SET_TEMPO;
  *_midFile++=3;
  _tempo=60000000/_tempo;
  *_midFile++=(_tempo&0x00FF0000)>>16;
  *_midFile++=(_tempo&0x0000FF00)>>8;
  *_midFile++=(_tempo&0x000000FF);
  size+=7;

  //meta event time signature
  if (size>=_chunk_size){ _midFile=(char*)realloc(_midFile, _chunk_size+100);_chunk_size+=100;}
  *_midFile++=0;
  *_midFile++=MIDI_META;
  *_midFile++=MIDI_TIME_SIGNATURE;
  *_midFile++=4;
  *_midFile++=1;
  *_midFile++=0;
  *_midFile++=24;
  *_midFile++=8;
  size+=8;

  //meta event key signature
  if (size>=_chunk_size){ _midFile=(char*)realloc(_midFile, _chunk_size+100);_chunk_size+=100;}
  *_midFile++=0;
  *_midFile++=MIDI_META;
  *_midFile++=MIDI_KEY_SIGNATURE;
  *_midFile++=2;
  *_midFile++=0;
  *_midFile++=0;
  size+=6;

  //meta event end of track
  if (size>=_chunk_size){ _midFile=(char*)realloc(_midFile, _chunk_size+100);_chunk_size+=100;}
  *_midFile++=0;
  *_midFile++=MIDI_META;
  *_midFile++=MIDI_END_OF_TRACK;
  *_midFile++=0;
  size+=4;

  *p_size=size-8;

  _midFile=(char*)realloc(_midFile, size);
  _chunk_size=size;
  for (int i=0; i<size; i++) {
    printf(" %02x", ((unsigned char *)head_midFile)[i]);
  }
  printf("\n");
  return head_midFile;
}

char Midi_track::get_chunk_size() {
  return _chunk_size;
}

void Midi_track::set_chunk_size(DWORD chunk_size) {
  _chunk_size=chunk_size;
}

char* Midi_track::get_midFile() {
  return _midFile;
}

void Midi_track::set_midFile(char* midFile) {
  _midFile=midFile;
}

std::string Midi_track::get_track_name() {
  return _track_name;
}

char * Midi_track::add_track() {
  DWORD size=0;
  char *head_midFile=_midFile;
  for (unsigned int i=0; i<_chunk_size; i++) {
    head_midFile[i]=0x0;
  }

  //chunk ID
  if (size>=_chunk_size){ _midFile=(char*)realloc(_midFile, _chunk_size+100);_chunk_size+=100;}
  *_midFile++='M';
  *_midFile++='T';
  *_midFile++='r';
  *_midFile++='k';
  size+=4;

  //chunk size
  if (size>=_chunk_size){ _midFile=(char*)realloc(_midFile, _chunk_size+100);_chunk_size+=100;}
  *_midFile++=0;
  *_midFile++=0;
  *_midFile++=0;
  char *p_size=_midFile;
  *_midFile++=size;
  size+=4;

  //meta event track name
  if (size>=_chunk_size){ _midFile=(char*)realloc(_midFile, _chunk_size+100);_chunk_size+=100;}
  *_midFile++=0;
  *_midFile++=MIDI_META;
  *_midFile++=MIDI_TRACK_NAME;
  *_midFile++=(char)_track_name.size();
  size+=4;
  for (unsigned int i=0; i<_track_name.size(); i++) {
    *_midFile++=(char)_track_name[i];
    size++;
  }
/*
  //event programme change event
  if (size>=_chunk_size){ _midFile=(char*)realloc(_midFile, _chunk_size+100);_chunk_size+=100;}
  *_midFile++=0;
  *_midFile++=MIDI_PROGRAM_CHANGE;
  *_midFile++=1;
  size+=3;

  //controller main volume event
  *_midFile++=0;
  *_midFile++=MIDI_CONTROLLER;
  *_midFile++=MIDI_MAIN_VOLUME;
  *_midFile++=100;
  size+=4;

  //event note on
  *_midFile++=0x87;
  *_midFile++=0x68;
  *_midFile++=MIDI_NOTE_ON;
  *_midFile++=C4;
  *_midFile++=100;
  size+=5;

  //event note off
  *_midFile++=0x83;
  *_midFile++=0x86;
  *_midFile++=0x50;
  *_midFile++=MIDI_NOTE_OFF;
  *_midFile++=C4;
  *_midFile++=100;
  size+=6;
*/
  //meta event end of track
  if (size>=_chunk_size){ _midFile=(char*)realloc(_midFile, _chunk_size+100);_chunk_size+=100;}
  *_midFile++=0;
  *_midFile++=MIDI_META;
  *_midFile++=MIDI_END_OF_TRACK;
  *_midFile++=0;
  size+=4;

  *p_size=size-8;
  _midFile=(char*)realloc(_midFile, size);
  _chunk_size=size;
  for (unsigned int i=0; i<size; i++) {
    printf(" %02x", ((unsigned char *)head_midFile)[i]);
  }
  printf("\n");
  return head_midFile;
}

void Midi_track::write_var(int var, char* midFile) {
  int var_tp=var;
  int size=0;
  int nb_octet=0;
  for (unsigned int i=0; i<(sizeof(int)*8); i++) {
    if (var_tp&1) size=i;
    var_tp=var_tp>>1;
  }
  nb_octet=(size/7)+1;
  for (int i=0; i<nb_octet; i++) {
    if (i==nb_octet-1) {
      *midFile++=var&0x7F;
    }
    else if (i==0) {
      *midFile++=((var>>(size-(size%7)))&(0x7F))|0x80;
      size-=size%7+7;
    }
    else {
      *midFile++=((var>>size)&0x7F)|0x80;
      size-=7;
    }
  }
}

/*
   int main(void) {
   FILE *file=fopen("prout.mid", "wb");
   Midi_head *zozo=new Midi_head(1, 2, 25, 2);
   char *head_midFile=zozo->write_header();
   fwrite(head_midFile, 1, zozo->get_size(), file);
   Midi_track *tata=new Midi_track("tata", 120);
   char *track0_midFile=tata->write_track0();
   fwrite(track0_midFile, 1, tata->get_chunk_size(), file);
   Midi_track *tonton=new Midi_track("tonton", 120);
   char *track_midFile=tonton->add_track();
   fwrite(track_midFile, 1, tonton->get_chunk_size(), file);
   fclose(file);

   return 0;
   }
   */
