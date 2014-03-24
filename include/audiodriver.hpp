

#ifndef ___AUDIODRIVER
#define ___AUDIODRIVER

#include <SFML/System.hpp> 
#include "config.hpp"
#include "signal.hpp"
#include "audiostream.hpp"

//An audio driver should have a single instance

class AudioDriver : public sf::NonCopyable
{
  public:
    AudioDriver() {}
    virtual ~AudioDriver() {}
    
    //Initialize the driver 
    //if the driver doesn't support rate he must use a supported rate and reconfigure 
    //signals by calling globalSignalConfiguration ! 
    //Be sure there is no thread with signal running when call this function
    virtual bool init(unsigned int rate)=0;
    
    //free the driver
    virtual void free()=0;
    
    //Start the driver thread
    //The driver should lock the stream when read from it to update
    //You must keep stream alive as long the thread is alive
    virtual bool start(AudioStream* stream)=0;
    
    //stop the audio thread
    virtual void stop()=0;
};

#endif