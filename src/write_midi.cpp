#include <stdio.h>
#include <stdlib.h>
#include "midi.hpp"

Midi_head::Midi_head(WORD format, WORD track, BYTE frame, BYTE ticks) :
  _midFile(NULL),
  _format(format),
  _track(track),
  _frame(frame),
  _ticks(ticks),
  _size(14)
{
  _midFile = (char*) malloc(_size);
}

Midi_head::~Midi_head() {
  free(_midFile);
}

char* Midi_head::write_header() {
  char *head_midFile=_midFile;
  for (int i=0; i<_size; i++) {
    head_midFile[i]=0;
  }
  for (int i=0; i<_size; i++) {
    printf(" %02x", head_midFile[i]);
  }
  printf("\n");
  *_midFile++='M';
  *_midFile++='T';
  *_midFile++='h';
  *_midFile++='d';
  *_midFile++=0;
  *_midFile++=0;
  *_midFile++=0;
  *_midFile++=6;

  if (_format!=1) {
    printf("le format n'est pas supporte\n");
  }
  else {
    *_midFile++=0;
    *_midFile++=1;
  }
  //nombre de piste
  *_midFile++=_track>>8;
  *_midFile++=(_track & 0x00FF);

  //resolution
  *_midFile++=0x80|_frame;
  *_midFile++=_ticks;

  for (int i=0; i<_size; i++) {
    printf(" %02x", ((unsigned char *)head_midFile)[i]);
  }
  printf("\n");
  return head_midFile;
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
  char size=0;
  char *head_midFile=_midFile;
  for (int i=0; i<_chunk_size; i++) {
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
  *_midFile++=META;
  *_midFile++=TRACK_NAME;
  *_midFile++=(char)_track_name.size();
  size+=4;
  for (int i=0; i<_track_name.size(); i++) {
    *_midFile++=(char)_track_name[i];
    size++;
  }

  //meta event set tempo
  if (size>=_chunk_size){ _midFile=(char*)realloc(_midFile, _chunk_size+100);_chunk_size+=100;}
  *_midFile++=0;
  *_midFile++=META;
  *_midFile++=SET_TEMPO;
  *_midFile++=3;
  _tempo=60000000/_tempo;
  *_midFile++=(_tempo&0x00FF0000)>>16;
  *_midFile++=(_tempo&0x0000FF00)>>8;
  *_midFile++=(_tempo&0x000000FF);
  size+=7;

  //meta event time signature
  if (size>=_chunk_size){ _midFile=(char*)realloc(_midFile, _chunk_size+100);_chunk_size+=100;}
  *_midFile++=0;
  *_midFile++=META;
  *_midFile++=TIME_SIGNATURE;
  *_midFile++=4;
  *_midFile++=1;
  *_midFile++=0;
  *_midFile++=24;
  *_midFile++=8;
  size+=8;

  //meta event key signature
  if (size>=_chunk_size){ _midFile=(char*)realloc(_midFile, _chunk_size+100);_chunk_size+=100;}
  *_midFile++=0;
  *_midFile++=META;
  *_midFile++=KEY_SIGNATURE;
  *_midFile++=2;
  *_midFile++=0;
  *_midFile++=0;
  size+=6;

  //meta event end of track
  if (size>=_chunk_size){ _midFile=(char*)realloc(_midFile, _chunk_size+100);_chunk_size+=100;}
  *_midFile++=0;
  *_midFile++=META;
  *_midFile++=END_OF_TRACK;
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

char Midi_head::get_size() {
  return _size;
}

char Midi_track::get_chunk_size() {
  return _chunk_size;
}

char * Midi_track::add_track() {
  char size=0;
  char *head_midFile=_midFile;
  for (int i=0; i<_chunk_size; i++) {
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
  *_midFile++=META;
  *_midFile++=TRACK_NAME;
  *_midFile++=(char)_track_name.size();
  size+=4;
  for (int i=0; i<_track_name.size(); i++) {
    *_midFile++=(char)_track_name[i];
    size++;
  }

  //event programme change event
  if (size>=_chunk_size){ _midFile=(char*)realloc(_midFile, _chunk_size+100);_chunk_size+=100;}
  *_midFile++=0;
  *_midFile++=PROGRAM_CHANGE;
  *_midFile++=1;
  size+=3;

  //controller main volume event
  *_midFile++=0;
  *_midFile++=CONTROLLER;
  *_midFile++=MAIN_VOLUME;
  *_midFile++=100;
  size+=4;

  //event note on
  *_midFile++=0x87;
  *_midFile++=0x68;
  *_midFile++=NOTE_ON;
  *_midFile++=C4;
  *_midFile++=100;
  size+=5;

  //event note off
  *_midFile++=0x83;
  *_midFile++=0x86;
  *_midFile++=0x50;
  *_midFile++=NOTE_OFF;
  *_midFile++=C4;
  *_midFile++=100;
  size+=6;

  //meta event end of track
  if (size>=_chunk_size){ _midFile=(char*)realloc(_midFile, _chunk_size+100);_chunk_size+=100;}
  *_midFile++=0;
  *_midFile++=META;
  *_midFile++=END_OF_TRACK;
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

