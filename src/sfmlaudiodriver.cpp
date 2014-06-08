

/*******************************************************
Nom ......... : sfmlaudiodriver.hpp
Role ........ : Drivers utilisant la OPENAL Via SFML
Auteur ...... : Julien DE LOOR
Version ..... : V2.0
Licence ..... : © Copydown™
********************************************************/

#include "config.hpp"

#ifndef NO_SFML_AUDIO
#include "sfmlaudiodriver.hpp"

SFMLAudioDriver::SFMLAudioDriver():
_stream(NULL),
_samples(NULL),
_samplesToStream(0),
_initialized(false),
_running(false)
{
  

}

SFMLAudioDriver::~SFMLAudioDriver()
{
  this->free();
}

bool SFMLAudioDriver::init(unsigned int rate)
{
  if (!_initialized){
    this->initialize(2,rate);
    _initialized=true;
    if (getSampleRate() != rate)
    {
      std::cout << "Warning: Cannot set ASIO sample rate to " << rate << "Hz, this will reconfigure alls signals frequency at " << getSampleRate() << "Hz now" << std::endl;
      Signal::globalConfiguration((unsigned int) getSampleRate() ,Signal::refreshRate);

    }
    _samplesToStream=Signal::size*2;
    _samples=(short*)std::malloc(_samplesToStream*2);
    std::cout << "SFML Driver initialized with sample rate " << getSampleRate() << "Hz\n";
    std::cout << "  Samples to Stream each : " << _samplesToStream << std::endl;    
  }
  return _initialized;
}

void SFMLAudioDriver::free(){
  this->stop();
  _initialized=false;
  if (_samples) std::free((void*)_samples);
  _samples=NULL;
}

bool SFMLAudioDriver::start(AudioStream* stream)
{
  if (!_initialized)
  {
    std::cerr << "SFMLAudioDriver error : start without init" << std::endl;   
    return false;
  }
  this->stop();

  _stream=stream;
  if (_stream) 
  {
    this->play();
    std::cout << "Start playing..." << std::endl;
    _running=true;
  }
  return _running;
}

void SFMLAudioDriver::stop()
{
  sf::SoundStream::stop();
  _running=false;
}

bool SFMLAudioDriver::onGetData(sf::SoundStream::Chunk& data)
{
  sf::Lock lock(*_stream);
  unsigned c = _stream->read(_samples,_samplesToStream);
  if (!c)
    std::cerr << "SFML Driver Critical : No samples to stream" << std::endl;  
  else 
    std::cout << "Deserve " << c << " Samples to sfml" << std::endl;
  data.samples=_samples;
  data.sampleCount=c;
  return true;
}

void SFMLAudioDriver::onSeek(sf::Time time)
{
  (void) time;
}

#endif
