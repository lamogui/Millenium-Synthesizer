
#ifndef __BASSDRIVER
#define __BASSDRIVER

#include "config.hpp"
#include "audiodriver.hpp"
#include "basserrorhandler.hpp"
#include "settings.hpp"

#include <bass.h>

class BassDriver : public AudioDriver
{
  public:
    BassDriver();
    virtual ~BassDriver();
    
    bool init(unsigned int rate);
    void free();
    bool start(AudioStream* stream);
    void stop();
    
  protected:
    HSTREAM _stream;
    
    bool _initialized;
    bool _running;
    
    static DWORD CALLBACK _StreamProc(HSTREAM handle,
                                      void *buffer,
                                      DWORD length,
                                      void *user);
};

#if defined(COMPILE_WINDOWS)
#include <bassasio.h>
class BassAsioDriver : public AudioDriver
{
  public:
    BassAsioDriver();
    virtual ~BassAsioDriver();
    
    bool init(unsigned int rate);
    void free();
    bool start(AudioStream* stream);
    void stop();
    
  protected:
    bool _initialized;
    bool _running;
    unsigned _right; //right channel
    unsigned _left;  //left channel
    
    static DWORD CALLBACK _AsioProc(BOOL input, 
                                    DWORD channel, 
                                    void *buffer, 
                                    DWORD length, 
                                    void *user);
};
#endif

#endif