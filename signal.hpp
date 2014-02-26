#ifndef __SIGNAL
#define __SIGNAL

#include <vector>
#include <cstdlib> //old malloc
#include <cstring> //memset


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
    
      //Constants for all signals
  static const unsigned int frequency = 44100;
  static const unsigned int channels = 2; //stereo only supported yet
  static const unsigned int refreshRate = 50; //Le but c'est d'avoir 50 ici ! le plus possible sera le mieux
  static const unsigned int size = channels*frequency/refreshRate;
  static const unsigned int byteSize = sizeof(sample)*size;
  static const unsigned int nBuffers = 4; //minimum 3 le but c'est d'en avoir le moins possible (ça augmente la latence)
};



#endif