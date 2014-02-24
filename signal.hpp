#ifndef __SIGNAL
#define __SIGNAL

#include <vector>
#include <cstdlib> //old malloc
#include <cstring> //memset
#include <bass.h>

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
    bool prepareNextBuffer(); //finnish prepare the current buffer
    
    //Play the buffer
    bool pop(); //Remove the first in buffer from the queue
    bool playInBass(HSTREAM stream); //push the current buffer in bass stream
    
    //Create the bass stream
    static HSTREAM CreateCompatibleBassStream();
      
  protected:
    std::vector<sample*> _buffers; //fifo/circular signal buffers
    unsigned int _prepared;  //Current prepared buffer
    unsigned int _played;   //Current played buffer
    unsigned int _filled;   //Number of filled buffer
};


#endif