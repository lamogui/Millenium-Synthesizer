
#ifndef __INSTRUMENT
#define __INSTRUMENT

#include "signal.hpp"
#include "note.hpp"
#include <vector>
#include <cstring>
#include <iostream>
#include <cmath>

//          PARAM NAME               MIDI ID   MIDI NAME
#define PARAM_INSTRUMENT_PAN_ID         0x0A   //Pan
#define PARAM_INSTRUMENT_VOLUME_ID      0x07   //Main Volume



//Class for manipulate a group of instrument
//only describes the methods that all instruments must have
//You should not inherit this class but inherit the Instrument class
//Use this class only for containers templates (std::vector<AbstractInstrument*>, etc)
class AbstractInstrument
{
  public:
    virtual ~AbstractInstrument() {};
    
    virtual void notify(InstrumentParameter* p) = 0; //notification d'un changement de valeur de parametre 
    virtual bool playNote(Note & n)=0;
    //if rightout then user want a mono signal
    virtual void step(Signal* leftout, Signal* rightout=0)=0;
    virtual InstrumentParameter* getParameter(unsigned char id)=0;
    inline bool setParameterValue(unsigned char id, short value)
    {
      InstrumentParameter* p = this->getParameter(id);
      if (p)
        return p->setValue(value);
      return false;
    }
};

//an instrument is divised in 2 part :
// - what it represent for a note : InstrumentVoice (dependant of the note)
// - what it represent for the whole sound : Instrument (dependant of global config)
//even if an instrument has only one voici he should provide both inherited class :  Instrument AND InstrumentVoice


//should represent all the material used by the instrument to produce one note
//To be sure that your voice class contains all requiert method copy paste this class
//You need to inherit this class to provide an interface with notes... 
class InstrumentVoice
{
  public:
    //Instruments Voice should only be created by instruments
    InstrumentVoice(AbstractInstrument* creator): visualize(false),  _instrument(creator), _used(false)
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
    //if rightout then user want a mono signal
    virtual void step(Signal* leftout, Signal* rightout=0)=0;
    
    //is this voice "visualized by something"
    bool visualize;
    
  protected:
    AbstractInstrument* _instrument; //Use the owner instrument to get parameters from
    bool _used;
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
    _volume(100,0,127),
    _pan(0,-127,127),
    _visu(0)
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
    
    virtual void notify(InstrumentParameter* p)
    {
      (void)p;
      //nothing to do
    }
    
    virtual bool playNote(Note & n)
    {
      for (unsigned int i=0; i < _voices.size(); i++)
      {
        if (!_voices[i]->isUsed())
        {
          _voices[i]->beginNote(n);
          n.receivePlayedSignal(_voices[i]);
          for (unsigned int k=0; k < _voices.size(); k++)
            _voices[i]->visualize=false;
          _voices[i]->visualize=true;
          _visu=_voices[i];
          return true;
        }
      }
      return false;
    }
    
    //if rightout then user want a mono signal
    virtual void step(Signal* leftout, Signal* rightout=0)
    {
      //reset output signal
      leftout->reset();
      if (rightout)
        rightout->reset();
      Signal current_left, current_right;
      
      //simple final mixing exemple
      if (rightout)
      {
        for (unsigned int i=0; i < _voices.size(); i++)
        {
          if (_voices[i]->isUsed())
          {
            _voices[i]->step(&current_left,&current_right);
            leftout->add(&current_left);
            rightout->add(&current_right);
          }
        }
      }
      else 
      {
        for (unsigned int i=0; i < _voices.size(); i++)
        {
          if (_voices[i]->isUsed())
          {
            _voices[i]->step(&current_left,0);
            leftout->add(&current_left);
          }
        }
      }
      const float s = (float)_volume.getValue()/(float)(sqrt(_voices.size()+1.f)*127.f*127.f);
      leftout->scale(s*(127+_pan.getValue()));
      if (rightout)
        rightout->scale(s*(127-_pan.getValue()));
    }
    
    virtual InstrumentParameter* getParameter(unsigned char id)
    {
      switch (id)
      {
        case PARAM_INSTRUMENT_PAN_ID: return &_pan;
        case PARAM_INSTRUMENT_VOLUME_ID: return &_volume;
        default: return NULL;
      }
    }
    
    inline voiceClass* getVisualVoice() {return _visu;}
     
  protected:
    std::vector<voiceClass*> _voices;
    InstrumentParameter _volume;
    InstrumentParameter _pan;
    voiceClass* _visu; //voice that the use "want to see" (last started)
};

#endif
