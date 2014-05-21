#include "fft.hpp" 

FFT::FFT(unsigned int size) :
 _values(NULL),
 _size(0),
 _pow2(FFT::getInfPow2(size)
{
  if (_pow2) {
    _size = 1 << _pow2;
    this->realloc(_size);
  }
}

FFT::~FFT() {
  if (_values) 
    free(_values);
}

static unsigned short FFT::getInfPow2(unsigned int v)
{
  unsigned short bit=0;
  for (unsigned int i=0; i < sizeof(unsigned int) << 3; i++)
  {
    if (v & 1) bit = i; v >>=1; 
  }
  return bit;
}
