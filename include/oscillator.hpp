#ifndef __OSCILLATORS
#define __OSCILLATORS

#include "signal.hpp"
#include <iostream>

class Oscillator : public AbstractSignalGenerator
{
  public:
    Oscillator();
    virtual ~Oscillator();
    
    //Must provide this
    virtual void step(Signal* output) = 0;

    
    //set constant frequency
    void setFrequency(float f);
    //set constant amplitude
    void setAmplitude(float a);
    //set constant unisson
    void setUnisson(float u);
    //set constant fm
    void setFM(float fm);
    //set constant shape
    void setShape(float s);
    
    
    //Used signals for parameters must be non-NULL
    //Function generate MUST always use these pointers
    //On doit virer ça... ça marche sans pour l'instant
    /*Signal* amplitude;
    Signal* frequency; 
    Signal* unisson;
    Signal* fm;
    Signal* shape;*/
    
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
    Signal _amplitude; //Oscillator "constant" amplitude;
    Signal _frequency; //Oscillator "constant" frequency;
    Signal _unisson; //Oscillator "constant" unison;
    Signal _fm; //Oscillator "constant" fmodulation;
    Signal _shape; //Oscillator "constant" shape (duty for square ...)
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

class SawOscillator : public Oscillator
{
  public:
    SawOscillator ();
    virtual ~SawOscillator ();
  
    virtual void step(Signal* output);
};

class TriangleOscillator : public Oscillator
{
  public:
    TriangleOscillator ();
    virtual ~TriangleOscillator ();
  
    virtual void step(Signal* output);
};

class WhiteNoiseOscillator : public Oscillator
{
  public:
    WhiteNoiseOscillator ();
    virtual ~WhiteNoiseOscillator ();
  
    virtual void step(Signal* output);
};

class RandomOscillator : public Oscillator
{
  public:
    RandomOscillator ();
    virtual ~RandomOscillator ();
  
    virtual void step(Signal* output);
    
  private:
    bool _flipped;
    float _last_value;
    
};

class RandomSmoothOscillator : public Oscillator
{
  public:
    RandomSmoothOscillator ();
    virtual ~RandomSmoothOscillator ();
  
    virtual void step(Signal* output);
  private:
    bool _flipped;
    float _last_value;
    float _y_1;
};

class TriplePeakOscillator : public Oscillator
{
  public:
    TriplePeakOscillator ();
    virtual ~TriplePeakOscillator ();
  
    virtual void step(Signal* output);

};

#endif
