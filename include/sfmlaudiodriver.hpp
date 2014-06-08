
/*******************************************************
Nom ......... : sfmlaudiodriver.hpp
Role ........ : Drivers utilisant la OPENAL Via SFML
Auteur ...... : Julien DE LOOR
Version ..... : V2.0
Licence ..... : © Copydown™
********************************************************/

#include "config.hpp"
#ifndef NO_SFML_AUDIO

#ifndef __SFMLAUDIODRIVER
#define __SFMLAUDIODRIVER

#include "audiodriver.hpp"
#include "settings.hpp"
#include <SFML/Audio.hpp>

class SFMLAudioDriver : public AudioDriver, public sf::SoundStream
{
  public:
    SFMLAudioDriver();
    virtual ~SFMLAudioDriver();
    
    bool init(unsigned int rate);
    void free();
    bool start(AudioStream* stream);
    void stop();
    
    //bool haveStalled();

    virtual bool onGetData(sf::SoundStream::Chunk& data);
    virtual void onSeek(sf::Time time);
    
  protected:
    AudioStream* _stream;
    short* _samples;    
    unsigned int _samplesToStream;
    bool _initialized;
    bool _running;
    
};

#endif
#endif
