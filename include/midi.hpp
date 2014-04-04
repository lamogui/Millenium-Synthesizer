#ifndef _MIDI_HPP
#define _MIDI_HPP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <bass.h>

class Midi {

private:
   char *_file;
   DWORD _chunk_header_size;
   WORD _format_type;
   WORD _number_track;
   WORD _time_division;
   
   
}

#ifndef