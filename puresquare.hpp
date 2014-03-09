#ifndef __PURESQUARE
#define __PURESQUARE

#include "instrument.hpp"
#include "oscillator.hpp"

class PureSquareVoice : public InstrumentVoice
{
  public:
    //Instruments Voice should only be created by instruments
    PureSquareVoice(AbstractInstrument* creator);
    virtual ~PureSquareVoice()
    
    virtual void beginNote(Note& n);
    virtual void endNote();
    
    virtual void step(Signal* output);
    
  protected:
    
};

#endif

