
#ifndef __INSTRUMENT
#define __INSTRUMENT

#include "signal.hpp"
#include "note.hpp"
#include <vector>
#include <cstring>
#include <iostream>
#include <cmath>

#define PARAM_INSTRUMENT_PAN_ID 254
#define PARAM_INSTRUMENT_VOLUME_ID 255

//an instrument is divised in 2 part :
// - what it represent for a note : InstrumentVoice (dependant of the note)
// - what it represent for the whole sound : Instrument (dependant of global config)
//even if an instrument has only one voici he should provide both inherited class :  Instrument AND InstrumentVoice

//Class for manipulate a group of instrument
//only describes the methods that all instruments must have
//You should not inherit this class but inherit the Instrument class
//Use this class only for containers templates (std::vector<AbstractInstrument*>, etc)
class AbstractInstrument : public AbstractSignalGenerator
{
  public:
    virtual ~AbstractInstrument() {};
    virtual bool playNote(Note & n)=0;
    virtual void step(Signal* output)=0;
    virtual InstrumentParameter* getParameter(unsigned char id)=0;
    inline bool setParameterValue(unsigned char id, short value)
    {
      InstrumentParameter* p = this->getParameter(id);
      if (p)
        return p->setValue(value);
      return false;
    }
};


//should represent all the material used by the instrument to produce one note
//To be sure that your voice class contains all requiert method copy paste this class
//You need to inherit this class to provide an interface with notes... 
class InstrumentVoice : public AbstractSignalGenerator
{
  public:
    //Instruments Voice should only be created by instruments
    InstrumentVoice(AbstractInstrument* creator): _instrument(creator), _used(false), _output()
    {
    }
    
    virtual ~InstrumentVoice()
    {
    }
    
    //is the voice currently used by the instrument for produce a note ?
    //may change in a future
    inline bool isUsed()
    {
      return _used;
    }
    
    virtual void beginNote(Note& n)=0;
    virtual void endNote()=0;
    
    virtual void step(Signal* output)=0;
    
  protected:
    AbstractInstrument* _instrument; //Use the owner instrument to get parameters from
    bool _used;
    Signal _output;
};

//all instrument should inherit from this class with  
//the instrument voice class as voiceClass
//If you don't want a "special final mixing" or parameters just use 
//Instrument<myVoiceClass>
template <class voiceClass>
class Instrument : public AbstractInstrument
{
  public:
    Instrument(unsigned int nbVoice=24) :
    _volume(150,0,255)
    {
      for (unsigned int i=0; i < nbVoice; i++)
      {
        _voices.push_back(new voiceClass(this));
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
      output->reset();
      //simple final mixing exemple
      for (unsigned int i=0; i < _voices.size(); i++)
      {
        if (_voices[i]->isUsed())
        {
          //std::cout << "playing voice " << i << std::endl; 
          output->add(_voices[i]->generate());
        }
      }
      output->scale(_volume.getValue()/(sqrt(_voices.size()+1.0)*255.0));
    }
    
    virtual InstrumentParameter* getParameter(unsigned char id)
    {
      if (id == PARAM_INSTRUMENT_VOLUME_ID) return &_volume;
      return NULL;
    }

  protected:
    std::vector<voiceClass*> _voices;
    InstrumentParameter _volume;
};

#endif