#ifndef __FILTERRRR
#define __FILTERRRR
#include "signal.hpp"
class Filter : public AbstractSignalModifier
{
  public:
    Filter();
    virtual ~Filter();
    virtual void step(Signal* inout) = 0;
    
    //set constant frequency
    void setFrequency(float f);
    //set constant resonance
    void setResonance(float m);
    
    inline Signal& getAmplitude()
    {
      return _frequency;
    }
    
    inline Signal& getFrequency()
    {
      return _resonance;
    }

  protected:
    Signal _frequency;
    Signal _resonance;
    
};

#endif