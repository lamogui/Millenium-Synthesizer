
#include "audiostream.hpp"
#include <iostream>

AudioStream::AudioStream() :
_buffers(),
_prepared(0),
_played(0),
_filled(0),
_stream(0),
_callbackMutex(),
_relativeDelta(0)
{
  for (unsigned int i=0; i < AudioStream::nBuffers;i++)
  {
    _buffers.push_back(new Signal);
  }
}

AudioStream::~AudioStream()
{
  if (_stream) BASS_StreamFree(_stream);
  for (unsigned int i=0; i < _buffers.size();i++)
  {
    delete _buffers[i];
  }
}


Signal* AudioStream::getPreparedBuffer()
{
  return _buffers[_prepared];
}

bool AudioStream::prepareNextBuffer()
{
  if (_filled==_buffers.size())
    return false;
  _prepared++;
  _filled++;
  if (_prepared==_buffers.size())
    _prepared=0;
  return true;
}

void AudioStream::_pop()
{
  if (_filled)
  {
    _played++;
    _filled--;
    if (_played==_buffers.size())
      _played=0;
  }
}

bool AudioStream::pushInBass()
{
  sf::Lock lock(_callbackMutex); 
   
  if (!_filled) return false;
  
  /*DWORD size = BASS_StreamPutData(_stream,(void*) _buffers[_played],0); //get queue size
  if (size < (Signal::byteSize*AudioStream::nBuffers) >> 1)
  {*/
    DWORD size = BASS_StreamPutData(_stream,(void*) (((unsigned char*)_buffers[_played]->samples) + _relativeDelta),Signal::byteSize-_relativeDelta);
    if (size == -1)
    {
      DWORD error = BASS_ErrorGetCode();
      std::cerr << "BASS_StreamPutData Error " << error << " [Signal::pushInBass]: ";
      switch (error)
      {
        case BASS_ERROR_HANDLE:
          std::cerr << "handle is not valid." << std::endl;
          break;
        case  BASS_ERROR_NOTAVAIL:
          std::cerr << "The stream is not using the push system" << std::endl;
          break;
        case  BASS_ERROR_ILLPARAM:
          std::cerr << "length is not valid, it must equate to a whole number of samples." << std::endl;
          break;
        case  BASS_ERROR_ENDED:
          std::cerr << "The stream has ended" << std::endl;
          break;
        case  BASS_ERROR_MEM:
          std::cerr << "There is insufficient memory." << std::endl;
          break;
        default:
          std::cerr << "unknown error probably wrong dll" << std::endl;
          break;
      }
      return false;
    }
    else 
    {
      if (size >= Signal::byteSize-_relativeDelta)
      {
        _pop(); _relativeDelta=0;
      }
      else
      {
        //std::cout << "relative delta warning ! excepted " <<  Signal::byteSize-_relativeDelta << " got " << size << std::endl;
        _relativeDelta+=Signal::byteSize;
      }
      return true;
    }
  //}
  return false;
}

HSTREAM AudioStream::createCompatibleBassStream(bool callback)
{
  DWORD flags = 0;
  AudioStream* user = 0;
  STREAMPROC* proc = STREAMPROC_PUSH;
  if (sizeof(sample)==sizeof(float)) flags |= BASS_SAMPLE_FLOAT;
  else if (sizeof(sample)==sizeof(unsigned char)) flags |= BASS_SAMPLE_8BITS;
  if (callback) 
  {
    proc = _BassStreamProc;
    user = this;
  }
  if (_stream) BASS_StreamFree(_stream);
  
  _stream = BASS_StreamCreate(Signal::frequency,
                              Signal::channels,flags,
                              proc,(void*) user);
  if (!_stream)
  {
    DWORD error = BASS_ErrorGetCode();
    std::cerr << "BASS_StreamCreate Error " << error << " [Signal::createCompatibleBassStream]: ";
    switch (error)
    {
      case BASS_ERROR_INIT:
        std::cerr << "BASS_Init has not been successfully called." << std::endl;
        break;
      case  BASS_ERROR_FORMAT:
        std::cerr << "The sample format is not supported by the device/drivers. If the stream is more than stereo or the BASS_SAMPLE_FLOAT flag is used, it could be that they are not supported. " << std::endl;
        break;
      case  BASS_ERROR_SPEAKER:
        std::cerr << "The specified SPEAKER flags are invalid. The device/drivers do not support them, they are attempting to assign a stereo stream to a mono speaker or 3D functionality is enabled. " << std::endl;
        break;
      case  BASS_ERROR_MEM:
        std::cerr << "There is insufficient memory." << std::endl;
        break;
      case  BASS_ERROR_NO3D:
        std::cerr << "Could not initialize 3D support." << std::endl;
        break;
      case  BASS_ERROR_UNKNOWN:
        std::cerr << "Some other mystery problem!" << std::endl;
        break;
      default:
        std::cerr << "unknown error probably wrong dll" << std::endl;
        break;
    }
  }
  else if (!callback)
  {
    //preload 
    
    DWORD size = BASS_StreamPutData(_stream,NULL,Signal::byteSize*AudioStream::nBuffers);
    if (size == -1)
    {
      DWORD error = BASS_ErrorGetCode();
      std::cerr << "BASS_StreamPutData Error " << error << " [Signal::createCompatibleBassStream]: ";
      switch (error)
      {
        case BASS_ERROR_HANDLE:
          std::cerr << "handle is not valid." << std::endl;
          break;
        case  BASS_ERROR_NOTAVAIL:
          std::cerr << "The stream is not using the push system" << std::endl;
          break;
        case  BASS_ERROR_ILLPARAM:
          std::cerr << "length is not valid, it must equate to a whole number of samples." << std::endl;
          break;
        case  BASS_ERROR_ENDED:
          std::cerr << "The stream has ended" << std::endl;
          break;
        case  BASS_ERROR_MEM:
          std::cerr << "There is insufficient memory." << std::endl;
          break;
        default:
          std::cerr << "unknown error probably wrong dll" << std::endl;
          break;
      }
      
      return 0;
    }
  }
  return _stream;
}

HSTREAM AudioStream::getBassStream()
{
  return _stream;
}

Signal* AudioStream::usePlayableBuffer()
{
  _callbackMutex.lock();
  if (!_filled) //no buffer prepared for stream !
  {
    _callbackMutex.unlock();
    return 0;
  }
  return _buffers[_played];
}

void AudioStream::releasePlayableBuffer()
{
  _callbackMutex.unlock();
  _pop();
}

DWORD CALLBACK AudioStream::_BassStreamProc(HSTREAM handle,
                                        void *buffer,
                                        DWORD length,
                                        void *user)
{
  AudioStream* s=(AudioStream*)user;
  DWORD writed=0;
  //std::cout << "Debug buffer needed " << length;
  if (s)
  {
    sample* samples = 0;
    sf::Lock(s->_callbackMutex);

    while (s->_filled && length)
    {
      DWORD will_write=0;
      if (length < Signal::byteSize - s->_relativeDelta) will_write = length;
      else will_write = Signal::byteSize - s->_relativeDelta;
      
      samples = s->_buffers[s->_played]->samples;
      memcpy((void*) (((unsigned char*)buffer) + writed),(void*) (((unsigned char*)samples) + s->_relativeDelta),will_write);
      if (will_write == Signal::byteSize-s->_relativeDelta) //end of buffer
      {
        s->_pop();s->_relativeDelta=0;
      }
      length-=will_write;
      writed+=will_write;
    }
  }
  //std::cout << " buffer given " << writed << std::endl;
  return writed;
}

