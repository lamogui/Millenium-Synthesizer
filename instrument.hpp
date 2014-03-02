
#ifndef __INSTRUMENT
#define __INSTRUMENT

#include "signal.hpp"
#include "note.hpp"
#include <vector>
#include <cstring>
#include <iostream>
//an instrument is divised in 2 part :
// - what it represent for a note : InstrumentVoice (dependant of the note)
// - what it represent for the whole sound : Instrument (dependant of global config)
//even if an instrument has only one voici he should provide both inherited class :  Instrument AND InstrumentVoice

class InstrumentVoice
//should represent all the material used by the instrument to produce one note
//To be sure that your voice class contains all requiert method copy paste this class
//You need to inherit this class to provide an interface with notes... 
{
  public:
    InstrumentVoice(): _used(false), _output()
    {
    }
    
    virtual ~InstrumentVoice()
    {
    }
    
    
    //is the voice currently used by the instrument for produce a note ?
    //may chnge in a future
    inline bool isUsed()
    {
      return _used;
    }
    
    virtual void beginNote(Note& n)=0;
    virtual void endNote()=0;
    
    virtual void step(Signal* output)=0;
    
    inline Signal& generateOutput()
    {
      step(&_output);
      return _output;
    }
    
  protected:
    bool _used;
    Signal _output;
    
};


//all instrument should inherit from this class with  
//the instrument voice class as voiceClass
template <class voiceClass>
class Instrument
{
  public:
    Instrument(unsigned int nbVoice=24)
    {
      for (unsigned int i=0; i < nbVoice; i++)
      {
        _voices.push_back(new voiceClass);
      }
    }
    
    virtual ~Instrument()
    {
      for (unsigned int i=0; i < _voices.size(); i++)
      {
        delete _voices[i];
      }
    }
    
    virtual bool playNote(Note & n)
    {
      for (unsigned int i=0; i < _voices.size(); i++)
      {
        if (!_voices[i]->isUsed())
        {
          _voices[i]->beginNote(n);
          n.receivePlayedSignal(_voices[i]);
          return true;
        }
      }
      return false;
    }
    
    virtual void step(Signal* output)
    {
      //reset output signal
      memset((void*) output->samples,0,Signal::byteSize);
      //simple final mixing exemple
      for (unsigned int i=0; i < _voices.size(); i++)
      {
        if (_voices[i]->isUsed())
        {
          //std::cout << "playing voice " << i << std::endl; 
          output->add(&_voices[i]->generateOutput());
        }
      }
      output->scale(1.0/sqrt(_voices.size()+1.0));
    }
    
    inline Signal& generateOutput()
    {
      step(&_output);
      return _output;
    }

  protected:
    std::vector<voiceClass*> _voices;
    Signal _output;
};

#endif