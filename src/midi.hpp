#include "../include/bass.h"
#include <string>

class Midi_head {
  public:
    Midi_head(WORD format, WORD track, BYTE frame, BYTE ticks);
    ~Midi_head();
    char * write_header();
    char get_size();
  private:
    char *_midFile;
    WORD _format;
    WORD _track;
    BYTE _frame;
    BYTE _ticks;
    char _size;
};

class Midi_track {
  public:
    Midi_track(const std::string &track_name, DWORD tempo);
    ~Midi_track();
    char* write_track0();
    char* add_track();
    char get_chunk_size();
  private:
    char *_midFile;
    std::string _track_name;
    DWORD _tempo;
    DWORD _chunk_size;
};

#define META 0xFF
#define TRACK_NAME 0x03
#define SET_TEMPO 0x51
#define TIME_SIGNATURE 0x58
#define KEY_SIGNATURE 0x59
#define END_OF_TRACK 0x2F

