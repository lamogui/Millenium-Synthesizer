

#include "signal.hpp"

Signal::Signal() :
_buffers(),
_prepared(0),
_played(0),
_filled(0)
{
  for (unsigned int i=0; i < Signal::nBuffers;i++)
  {
    _buffers.push_back((float*) malloc(Signal::byteSize));
    memset((void*) _buffers[i],0,Signal::byteSize);
  }
}

Signal::Signal(const Signal& s) :
_buffers(),
_prepared(s._prepared),
_played(s._played),
_filled(s._filled)
{
  for (unsigned int i=0; i < s._buffers.size();i++)
  {
    _buffers.push_back((float*) malloc(Signal::byteSize));
    memcpy((void*) _buffers[i],(void*) s._buffers[i],Signal::byteSize);
  }
}

Signal::~Signal()
{
  for (unsigned int i=0; i < _buffers.size();i++)
  {
    free((void*) _buffers[i]);
  }
}


sample* Signal::getPreparedBuffer()
{
  return _buffers[_prepared];
}

bool Signal::prepareNextBuffer()
{
  if (_filled==_buffers.size())
    return false;
  _prepared++;
  if (_prepared==_buffers.size())
    _prepared=0;
  return true;
}

bool Signal::pop()
{
  if (!_filled) //no buffer prepared for stream !
    return false;
  _played++;
  if (_played==_buffers.size())
    _played=0;
  return true;
}

bool Signal::playInBass(HSTREAM stream)
{
  if (!_filled) return false;
  DWORD size = BASS_StreamPutData(stream,_buffers[_played],Signal::byteSize);
  
  return pop();
}

