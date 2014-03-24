#ifndef __SIGNAL
#define __SIGNAL

#include <vector>
#include <cstdlib> //old malloc
#include <cstring> //memset
#include <set>

typedef float sample; //type pour un echantillon

class Signal
{
  public:
    Signal();
    Signal(const Signal& s);
    virtual ~Signal(); 
      
      //Signal take care of allocation of this
    sample* samples;

    void mix(const Signal*, unsigned int n=1);
    void add(const Signal*, unsigned int n=1);
    void addOffset(sample offset);
    void scale(sample scale);
    void constant(sample constant);
    void saturate(sample min, sample max);
    
    
    
    inline void reset()
    {
      memset((void*) samples,0,Signal::byteSize);
    }
    
    
    Signal& operator=(const Signal&);
    
      //Constants for all signals
  static unsigned int frequency;
  static unsigned int channels; //stereo only supported yet
  static unsigned int refreshRate; //Le but c'est d'avoir 50 ici ! le plus possible sera le mieux
  static unsigned int size;
  static unsigned int byteSize;
  
  
  //be sure to lock ALL instances of Signals before do this
  static void globalConfiguration( unsigned int f,  unsigned int r);
  
  private:
    void _reset();
    static std::set<Signal*> _instances;
  
};

class AbstractSignalModifier
{
  public:
    virtual ~AbstractSignalModifier() {};
    virtual void step(Signal* inout) = 0;
};

class AbstractSignalGenerator : public AbstractSignalModifier
{
  public:
    AbstractSignalGenerator() : _time(0) {};
    virtual ~AbstractSignalGenerator() {};
    
    virtual void step(Signal* output) = 0;
    
    inline Signal* generate()
    {
      this->step(&_output);
      return &_output;
    }
    
    virtual inline void resetTime()
    {
      _time=0;
    }
   
  protected:
    Signal _output;
    unsigned int _time; //in (1/fe) secondes
};



#endif