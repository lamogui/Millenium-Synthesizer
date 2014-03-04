#ifndef __OSCILLATORS
#define __OSCILLATORS

#include "signal.hpp"

class Oscillator
{
  public:
    Oscillator();
    virtual ~Oscillator();
    
    inline Signal& generate()
    {
      this->step(&_output);
      return _output;
    }
    
    //Must provide this
    virtual void step(Signal* output) = 0;

    
    inline void resetTime()
    {
      _time=0;
    }
    
    //set constant frequency
    virtual void setFrequency(float f);
    //set constant amplitude
    virtual void setAmplitude(float a);
    //set constant unisson
    virtual void setUnisson(float u);
    //set constant fm
    virtual void setFM(float fm);
    //set constant shape
    virtual void setShape(float s);
    
    //Used signals for parameters must be non-NULL
    //Function generate MUST always use these pointers
    Signal* amplitude;
    Signal* frequency; //
    Signal* unisson;
    Signal* fm;
    Signal* shape;
    
    //if the user want use the internal Signal for 
    //with specific wave and not constant
    inline Signal& getAmplitude()
    {
      return _amplitude;
    }
    
    inline Signal& getFrequency()
    {
      return _frequency;
    }
    
    inline Signal& getUnisson()
    {
      return _unisson;
    }
    
    inline Signal& getFM()
    {
      return _fm;
    }
    
    inline Signal& getShape()
    {
      return _shape;
    }

  protected:
    unsigned int _time; //in (1/fe) secondes
 
  private:
    Signal _amplitude; //Oscillator "constant" amplitude;
    Signal _frequency; //Oscillator "constant" frequency;
    Signal _unisson; //Oscillator "constant" unison;
    Signal _fm; //Oscillator "constant" fmodulation;
    Signal _shape; //Oscillator "constant" shape (duty for square ...)
    Signal _output;
};


class SinusoidalOscillator : public Oscillator
{
  public:
    SinusoidalOscillator();
    virtual ~SinusoidalOscillator();
  
    virtual void step(Signal* output);
};

class SquareOscillator : public Oscillator
{
  public:
    SquareOscillator ();
    virtual ~SquareOscillator ();
  
    virtual void step(Signal* output);
};

#endif
