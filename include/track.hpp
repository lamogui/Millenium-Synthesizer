#ifndef ___TRACK
#define ___TRACK

#include "instrument.hpp"

#include <vector>
#include <fstream>
#include "bass.h"

//Define an instrument track which contains event and notes
class Track
{
  public:
    Track();
    Track(AbstractInstrument* i);
    ~Track();
    
    
    //Try to load the track from midi file format
    bool loadFromMidi(std::ifstream& file, WORD time_division, float &bpm);
   
    //try to set the track at the time position 
    // return false is the time is out of bounds   
    bool seek(unsigned int time);
   
    //Increment of 1 unit of time (1/frequency sec) and add/remove notes from the instrument 
    // return true if the track has ended
    bool tick();
    
    
    //Stop all current played notes 
    void panic();
    
    inline void setInstrument(AbstractInstrument* i) 
    {
      _instrument=i;
    }
  
  private:
    AbstractInstrument* _instrument;
    unsigned int _time;
    unsigned int _currentNote;
    unsigned int _currentEvent;
    
    
    std::vector<InstrumentParameterEvent> _events;
    std::vector<Note> _notes;
    std::vector<Note*> _played;
    
};

#endif