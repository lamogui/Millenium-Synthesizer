#include "bass.h"
#include <string>
#include <cstdio>
#include <cstdlib>

class Midi_head {
  public:
    Midi_head(WORD format, WORD tracks, BYTE frame, BYTE ticks);
    ~Midi_head();
    
    bool write_to_buffer(unsigned char* buffer, unsigned int size );
    bool write_to_file(FILE* file);
    static const unsigned size=14;
    
  private:
    WORD _format;
    WORD _tracks;
    BYTE _frame;
    BYTE _ticks;
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
    void write_var(int var);
  private:
    char *_midFile;
    std::string _track_name;
    DWORD _tempo;
    DWORD _chunk_size;
};

#define MIDI_META 0xFF
#define MIDI_TRACK_NAME 0x03
#define MIDI_SET_TEMPO 0x51
#define MIDI_TIME_SIGNATURE 0x58
#define MIDI_KEY_SIGNATURE 0x59
#define MIDI_END_OF_TRACK 0x2F
#define MIDI_PROGRAM_CHANGE 0xC0
#define MIDI_CONTROLLER 0xB0
#define MIDI_MAIN_VOLUME 0x07
#define MIDI_NOTE_ON 0x90
#define MIDI_NOTE_OFF 0x80
