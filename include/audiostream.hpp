
#include "config.hpp"
#include "signal.hpp"
#include <cstdlib>
#include <SFML/System.hpp>

/*
#if defined(COMPILE_WINDOWS)
#include "bassasio.hpp"
#endif

#include "bass.h"*/


//AudioStream is just a fixed size FIFO buffer
// IN -> audio to diffuse
// OUT -> audio diffused
//But it can be used in invert
//You can lock() an audioStream before use it if you use threads
//All length variables are in samples (not in bytes)
class AudioStream : public sf::Mutex
{
  public:
    AudioStream(unsigned int buffer_length);
    virtual ~AudioStream(); 
    
    //you can resize the buffer only if there is no samples inside (all samples are free)
    bool setBufferLength(unsigned int buffer_length);
    
    //return buffer size in samples
    inline unsigned int getBufferLength()
    {
      return _length;
    }
    
    //try to read samples into a buffer, return the number of samples readed
    unsigned int read(unsigned short* buffer, unsigned int len);
    //try to write samples into the buffer, return the number of samples writed
    unsigned int write(const unsigned short* buffer, unsigned int len);
    
    //try to write an entire signal into the buffer return true if succeed
    bool writeSignal(const Signal& signal); 
    bool writeStereoSignal(const Signal& left,const Signal& right); 
    //try to read an entire signal from the buffer return true if succeed
    //bool readSignal(Signal& signal); //TODO
    
    inline unsigned int getAvailableSamplesCount()
    {
      return _count;
    }    
    inline unsigned int getFreeSamplesCount()
    {
      return _length-_count;
    }

  protected:
    unsigned short* _buffer;
    unsigned short* _end; //end of the buffer
    unsigned short* _pwrite; // pointeur d’écriture
    unsigned short* _pread; // pointeur de lecture
    unsigned int _length; //size in samples
    unsigned int _count; //nombre de samples disponibles
  
};


/*
class AudioStream
{
  public:
    AudioStream();
    virtual ~AudioStream();  
 
    static const unsigned int nBuffers = 3; //minimum 3 le but c'est d'en avoir le moins possible (ça augmente la latence)
 
    //Get an access to the buffers
    //NON-THREAD SAFE BUT YOU SHOULD NOT WRITE WITH MULTIPLES THREADS
    //(SHOULD BE) SAFE WITH READING (WILL NOT ERASE THE CURRENT PLAYED BUFFER !)
    Signal* getPreparedBuffer(); //get current prepared buffer
    bool prepareNextBuffer(); //finnish prepare the current buffer
    
    //Play the playable buffer
    //DON'T USE 4 FOLLOWING WITH BASS CALLBACK STREAM !
    bool pushInBass(); //push the current buffer in bass stream (push method)
    Signal* usePlayableBuffer(); //use the current played buffer DON'T USE WITH BASS CALLBACK STREAM !
    void releasePlayableBuffer(); //release the current playable buffer and go to the next
    //DON'T forget to RELEASE the PlayableBuffer ! AND DON'T BE TIME EXCESSIVE BETWEEN USE AND RELEASE
    
    
    //Create the bass stream erase previous created
    HSTREAM createCompatibleBassStream(bool callback=true); //use callback method -> direct played into speakers and no need 
    HSTREAM getBassStream(); //getter
      
  protected:
    void _pop(); //Remove the first in buffer from the queue (because it has been played)
  
    std::vector<Signal*> _buffers; //fifo/circular signal buffers
    volatile unsigned int _prepared;  //Current prepared buffer
    volatile unsigned int _played;   //Current played buffer
    volatile unsigned int _filled;   //Number of filled buffer
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
    #if defined(COMPILE_WINDOWS)
    static DWORD CALLBACK _BassAsioProc(BOOL input, 
                                        DWORD channel, 
                                        void *buffer, 
                                        DWORD length, 
                                        void *user);
    #endif
};*/