#ifndef __FILTERRRR
#define __FILTERRRR
#include "signal.hpp"
class Filter : public AbstractSignalModifier
{
  public:
    Filter();
    Filter(float f, float m=0.5f);
    virtual ~Filter();
    virtual void step(Signal* inout) = 0;
    
    //set constant frequency
    void setFrequency(float f);
    //set constant resonance
    void setResonance(float m);
    
    inline Signal& getFrequency()
    {
      return _frequency;
    }
    
    inline Signal& getResonance()
    {
      return _resonance;
    }

  protected:
    Signal _frequency;
    Signal _resonance;
    
};

class LowPassFilter : public Filter
{
  public:
    LowPassFilter();
    LowPassFilter(float f, float m=0.5f);
    virtual ~LowPassFilter();
    virtual void step(Signal* inout);
    
  private:
    sample _y_1; //y(n-1)
};

class LowPassFilter2 : public Filter
{
  public:
    LowPassFilter2();
    LowPassFilter2(float f, float m=0.5f);
    virtual ~LowPassFilter2();
    virtual void step(Signal* inout);
    
  private:
    sample _y_1; //y(n-1)
    sample _y_2; //y(n-2)
    sample _x_1; //x(n-1)
};

#endif