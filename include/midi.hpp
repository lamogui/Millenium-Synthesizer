#include "bass.h"
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
    char* get_midFile();
    void set_chunk_size(DWORD chunk_size);
    void set_midFile(char* midFile);
    std::string get_track_name();
    void write_var(char* midFile, int var);
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
#define PROGRAM_CHANGE 0xC0
#define CONTROLLER 0xB0
#define MAIN_VOLUME 0x07
#define NOTE_ON 0x90
#define NOTE_OFF 0x80
#define C4 0x3C
