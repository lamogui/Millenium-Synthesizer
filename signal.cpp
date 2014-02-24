

#include "signal.hpp"

Signal::Signal() :
_size(0),
_frequency(0),
_buffer1(0),
_buffer2(0),      
_currentBuffer(0)
{
   

}

Signal::Signal(unsigned int size, unsigned int frequency) :
_size(size),
_frequency(frequency),
_buffer1(0),
_buffer2(0),      
_currentBuffer(0)
{
   _buffer1 = new float[size];
   _buffer2 = new float[size];
   _currentBuffer = _buffer1;
}

Signal::Signal(const Signal& s) :
_size(s._size),
_frequency(s._frequency),
_buffer1(0),
_buffer2(0),      
_currentBuffer(0)
{
   _buffer1 = new float[size];
   _buffer2 = new float[size];
   
   if (s._currentBuffer == s._buffer1)
   _currentBuffer = _buffer1;
   
   for (unsigned int i=0; i < size; i++)
   {
      _buffer1[i]=s._buffer1[i];
      _buffer2[i]=s._buffer2[i];
   }
}