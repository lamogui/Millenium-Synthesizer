#ifndef __SIGNAL
#define __SIGNAL

#include <vector>
#include <cstdlib> //old malloc
#include <cstring> //memset
#include "bass.h"
#include <SFML/System.hpp>

typedef float sample; //type pour un echantillon

class Signal
{
  public:
    Signal();
    //COPY CONSTRUCTOR IS NON-THREAD SAFE
    Signal(const Signal& s); //don't copy the bass stream associated ! need to regenerate it !
    virtual ~Signal();  
    
    //Constants for all signals
    static const unsigned int frequency = 44100;
    static const unsigned int channels = 2; //stereo only supported yet
    static const unsigned int refreshRate = 50; //Le but c'est d'avoir 50 ici ! le plus possible sera le mieux
    static const unsigned int size = channels*frequency/refreshRate;
    static const unsigned int byteSize = sizeof(sample)*size;
    static const unsigned int nBuffers = 4; //minimum 3 le but c'est d'en avoir le moins possible (ça augmente la latence)
    
    
    //Get an access to the buffers
    //NON-THREAD SAFE BUT YOU SHOULD NOT WRITE WITH MULTIPLES THREADS
    //(SHOULD BE) SAFE WITH READING (WILL NOT ERASE THE CURRENT PLAYED BUFFER !)
    sample* getPreparedBuffer(); //get current prepared buffer
    bool prepareNextBuffer(); //finnish prepare the current buffer
    
    //Play the playable buffer
    //DON'T USE 3 FOLLOWING WITH BASS CALLBACK STREAM !
    bool pushInBass(); //push the current buffer in bass stream (push method)
    sample* usePlayableBuffer(); //use the current played buffer DON'T USE WITH BASS CALLBACK STREAM !
    void releasePlayableBuffer(); //release the current playable buffer and go to the next
    
    //DON'T forget to RELEASE the PlayableBuffer ! AND DON'T BE TIME EXCESSIVE BETWEEN USE AND RELEASE
    
    
    //Create the bass stream erase previous created
    HSTREAM createCompatibleBassStream(bool callback=true); //use callback method -> direct played into speakers and no need 
    HSTREAM getBassStream(); //getter
      
  protected:
    void _pop(); //Remove the first in buffer from the queue (because it has been played)
  
    std::vector<sample*> _buffers; //fifo/circular signal buffers
    unsigned int _prepared;  //Current prepared buffer
    unsigned int _played;   //Current played buffer
    unsigned int _filled;   //Number of filled buffer
    HSTREAM _stream;        //Bass Stream
    
  private:
    //Callback mutex
    sf::Mutex _callbackMutex;
    
    //Delta into a buffer for _BassStreamProc
    unsigned int _relativeDelta;
  
    //The Bass Stream proc available 
    static DWORD CALLBACK _BassStreamProc(HSTREAM handle,
                                          void *buffer,
                                          DWORD length,
                                          void *user);
};


#endif