#include "signal.hpp"

Signal::Signal() :
samples(0)
{
  samples=(sample*) malloc(Signal::byteSize);
  memset((void*) samples,0,Signal::byteSize);
}

Signal::Signal(const Signal& s) :
samples(0)
{
  samples=(sample*) malloc(Signal::byteSize);
  memcpy((void*) samples,(void*) s.samples,Signal::byteSize);
}

Signal::~Signal()
{
  if (samples)
    free((void*) samples);
}

void Signal::mix(const Signal* s, unsigned int n)
{
  for (unsigned int k=0; k < n; k++)
  {
    for (unsigned int i=0; i < Signal::size;i++)
    {
      samples[i] *= s[k].samples[i];
    }
  }
}

void Signal::add(const Signal* s, unsigned int n)
{
  for (unsigned int k=0; k < n; k++)
  {
    for (unsigned int i=0; i < Signal::size;i++)
    {
      samples[i] += s[k].samples[i];
    }
  }
}



