
/****************************************************************************
Nom ......... : track.hpp
Role ........ : Definie une piste (partition de musique)
Auteur ...... : Julien DE LOOR & Kwon-Young CHOI
Version ..... : V1.0 olol
Licence ..... : © Copydown™
****************************************************************************/

#ifndef ___JAI_LE__TRACK
#define ___JAI_LE__TRACK

#include "instrument.hpp"
#include "midi.hpp"
#include <vector>
#include <fstream>
#include "bass.h"
#include "midi.hpp"
#include "config.hpp"
#include "interface.hpp"
#ifdef COMPILE_WINDOWS
#include <windows.h>
#else
#include <cstring>
#include <iostream>
#endif

//Define an instrument track which contains event and notes
class Track
{
  public:
    Track();
    Track(AbstractInstrument* i);
    ~Track();
    
   
    //try to set the track at the time position 
    // return false is the time is out of bounds   
    bool seek(unsigned int time);
   
    //Increment of 1 unit of time (1/frequency sec) and add/remove notes from the instrument 
    // return true if the track has reach end 
    bool tick();
    
    void reset();
    
    //add a note at the current time of the track... (KeyPressed or NoteOn)
    Note* recordNoteStart(unsigned char id, float v=1.0);
    
    //Finalize the note recording (KeyRelease or NoteOff)
    void recordNoteRelease(Note* n);
    
    //Stop all current played notes 
    void panic();
    
    //Return a fast approximation of track length always < or = to the real track length
    unsigned int fastLength();
    
    //concatene track_extern dans this
    bool concatenate(const Track &track_extern);

    bool importFromMidiTrack(const Midi_track& midi);
    void exportToMidiTrack(Midi_track& midi) const;

    //Return current track time in refreshRate Signal unit
    inline unsigned int time(){
      return _time;
    }
    
    inline void setInstrument(AbstractInstrument* i) 
    {
      panic();
      _instrument=i;
    }
  
  private:
    AbstractInstrument* _instrument;
    unsigned int _time;
    unsigned int _currentNote;
    unsigned int _currentEvent;
    mutable sf::Mutex _mutex;    
    
    std::vector<InstrumentParameterEvent*> _events;
    std::vector<Note*> _notes;
    std::vector<Note*> _played;
    
};

void SaveTrackToMIDIFileRoutine(const Track* t);
void OpenFromMIDIFileRoutine(Track* t);

#endif
