/*******************************************************
Nom ......... : midi.hpp
Role ........ : Déclare les classes ayant une relation avec le format MIDI
                voir : http://www.sonicspot.com/guide/midifiles.html
Auteur ...... : Kwon-Young CHOI & Julien DE LOOR
Version ..... : V1.0 olol
Licence ..... : © Copydown™
********************************************************/

#ifndef __MIDI_MINUIT
#define __MIDI_MINUIT

#include "bass.h"
#include <string>
#include <cstdio>
#include <cstdlib>
#include "signal.hpp"

class Midi_head {
  public:
    Midi_head(WORD format, WORD tracks, WORD tick_per_beat);
    Midi_head(WORD format, WORD tracks, BYTE frame, BYTE ticks);
    ~Midi_head();
    
    bool read_from_buffer(const unsigned char* buffer, unsigned int size );
    
    bool write_to_buffer(unsigned char* buffer, unsigned int size ) const;
    bool write_to_file(FILE* file) const;
    static const unsigned size=14; //size in the final file in bytes
    
    void print_infos() const ;
    
    inline float gain() const {if (!_gain) return 1.f; return _gain;}
    inline bool need_bpm() const { return _beat; }
    inline WORD format() const { return _format; }
    inline WORD tracks() const { return _tracks; }
    inline void set_tracks(WORD tracks) {  _tracks = tracks; }
    
  private:
    bool _beat;
    float _gain; //Represent the "precision tick gain" in relation to the signal refresh rate
                 //Valable pour le mode FPS
                 // Gain =  Midi Rate / Signal refreshRate
                 // Midi Rate = Signal refreshRate * Gain
                 // Midi Ticks = Signal Ticks * Gain
                 // Signal Ticks = Midi Ticks / Gain
                 //Valable pour le mode Beat
    WORD _format;
    WORD _tracks;
    WORD _division;
    
};

///MIDI Track 0 for format 1 ! 
class Midi_track0 {
  public:
    Midi_track0(); 
    Midi_track0(const std::string name); 
    ~Midi_track0(); 

    unsigned int size() const; //size in the final file in bytes
    unsigned int read_from_buffer(const unsigned char* buffer, unsigned int size);
    
    bool write_to_buffer(unsigned char* buffer, unsigned int size ) const;
    bool write_to_file(FILE* file) const;
    
    void print_infos() const ;
    
    ///Getters / Setters
    inline void set_bpm(unsigned bpm) { if (bpm) _mpqn=60000000/bpm;}
    inline void set_music_name(const std::string n) {_music_name=n;}
    inline void set_copyright(const std::string c) {_copyright=c;}
    inline void set_comment(const std::string c) {_comment=c;}
    
    inline std::string music_name() { return _music_name; }
    inline std::string copyright() { return _copyright; }
    inline std::string comment() { return _comment; }
    inline unsigned int bpm() { if (_mpqn) return  60000000/_mpqn; return 0;}
    
    inline void reset() {
      _music_name.clear();
      _copyright.clear();
      _comment.clear();
      _mpqn=0;
    }
    
  private:
    unsigned int _mpqn;
    std::string _music_name;
    std::string _copyright;
    std::string _comment;
};

class Midi_track {
  public:
    Midi_track(Midi_head& head);
    ~Midi_track();
    
    unsigned int size() const ; //size in the final file in bytes
    inline unsigned int chunk_size() const {
      return _chunk_size;
    }
    
    inline const unsigned char* get_chunk() const {
      return _chunk;
    }
    
    unsigned int read_from_buffer(const unsigned char* buffer, unsigned int size);
    
    bool write_to_buffer(unsigned char* buffer, unsigned int size ) const;
    bool write_to_file(FILE* file) const;
    
    void push_midi_event(DWORD midi_delta, BYTE type, BYTE chan, BYTE p1, BYTE p2);
    void push_midi_event(DWORD midi_delta, BYTE type, BYTE chan, BYTE p1);
    
    inline void reset() {_chunk_size=0;}
    
     inline const Midi_head& get_head() const { return *_head; }
    
  private:
    void push_varlength(DWORD var);
  
    inline void check_alloc(unsigned int wanted) 
    {
      if (_alloc_size < wanted+_chunk_size)
      {
        _alloc_size += wanted + 128;
        _chunk = (unsigned char*) realloc(_chunk,_alloc_size);
      }
    }
  
    Midi_head* _head;
    unsigned char* _chunk;    //buffer for track data
    unsigned int _alloc_size; //Now allocated size
    DWORD _chunk_size;        //Size of actual track datas
                              //Never include EndOfTrack
};

#define MIDI_META 0xFF
#define MIDI_TRACK_NAME 0x03
#define MIDI_SET_TEMPO 0x51
#define MIDI_TIME_SIGNATURE 0x58
#define MIDI_KEY_SIGNATURE 0x59
#define MIDI_END_OF_TRACK 0x2F
#define MIDI_PROGRAM_CHANGE 0xC
#define MIDI_CONTROLLER 0xB
#define MIDI_MAIN_VOLUME 0x07
#define MIDI_NOTE_ON 0x9
#define MIDI_NOTE_OFF 0x8


//// WARNING ZONE COMPLETE RECODING //// WARNING ZONE COMPLETE RECODING //// WARNING ZONE COMPLETE RECODING 
//// WARNING ZONE COMPLETE RECODING //// WARNING ZONE COMPLETE RECODING //// WARNING ZONE COMPLETE RECODING 

class Midi_var {
  public:
    Midi_var() : _var(0) {}
    Midi_var(unsigned int var) : _var(var) {}
    Midi_var(const Midi_var& m) : _var(m._var) {}
    ~Midi_var() {}

    inline unsigned int read_from_buffer(const unsigned char* buffer, 
                                         unsigned int size)
    {
      unsigned g=0;
      _var=0;
      while (size > g && buffer[g] & 0x80)
        _var = (_var << 7) | (buffer[g++] & 0x7F);
      if (size > g)
        _var = buffer[g++];
      return g;
    }
    
    inline unsigned int size() const
    {
      unsigned int bit=0;
      for (DWORD i=0; i<32; i++) {
        if ((_var >> bit)&1) bit=i;
      }
      return bit/7 + 1;
    }
    
    inline unsigned int write_to_buffer( unsigned char* buffer, 
                                         unsigned int size) const 
    {
      const unsigned int parts = this->size() - 1;
      unsigned g=0;
      if (size > parts) {
        for (unsigned int i=0; i<parts; i++) 
            buffer[g++] = ((_var>>(7*(parts-i))) & 0x7F) | 0x80;
        buffer[g++] = _var & 0x7F;
      }
      return g;                               
    }
    
    inline void set_var(unsigned int var) { _var=var;}
    inline unsigned int var() const { return _var;}
    
    inline operator unsigned int () { return _var;}
    inline Midi_var& operator=(const Midi_var& m) { _var=m._var; return *this;}
    inline unsigned int operator=(const unsigned int v) { _var=v; return v;}
    
  private:
    unsigned int _var;
};
/*
class Midi_event {
  public:
    Midi_event();
    virtual Midi_event();
    
    unsigned int delta;
    
  private:
    
};
*/



//// WARNING ZONE COMPLETE RECODING //// WARNING ZONE COMPLETE RECODING //// WARNING ZONE COMPLETE RECODING 
//// WARNING ZONE COMPLETE RECODING //// WARNING ZONE COMPLETE RECODING //// WARNING ZONE COMPLETE RECODING 

#endif
