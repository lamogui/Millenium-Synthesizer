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
    
    //Constants for all signals
    static const unsigned int frequency = 44100;
    static const unsigned int channels = 2; //stereo only supported yet
    static const unsigned int refreshRate = 50; 
    static const unsigned int size = channels*frequency/refreshRate;
    static const unsigned int byteSize = sizeof(sample)*size;
    static const unsigned int nBuffers = 4; //min 2
    
    
    //Get an access to the buffers
    sample* getPreparedBuffer(); //get current prepared buffer
    bool nextBuffer(); //finnish prepare the current buffer
    
    //Play the buffer
    
    
      
  protected:
    std::vector<sample*> _buffers; //fifo buffers of the signal
    unsigned int _prepared;  //Current prepared buffer
    unsigned int _played;   //Current played buffer
    unsigned int _filled;   //Number of filled buffer
};


#endif