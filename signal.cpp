#include "signal.hpp"

unsigned int Signal::frequency=44100;
unsigned int Signal::refreshRate= 50;
unsigned int Signal::channels=2;
unsigned int Signal::size=Signal::frequency*Signal::channels/Signal::refreshRate;
unsigned int Signal::byteSize=sizeof(sample)*Signal::size;

std::set<Signal*> Signal::_instances;

Signal::Signal() :
samples(0)
{
  samples=(sample*) malloc(Signal::byteSize);
  memset((void*) samples,0,Signal::byteSize);
  _instances.insert(this);
}

Signal::Signal(const Signal& s) :
samples(0)
{
  samples=(sample*) malloc(Signal::byteSize);
  memcpy((void*) samples,(void*) s.samples,Signal::byteSize);
  _instances.insert(this);
}

Signal::~Signal()
{
  if (samples)
    free((void*) samples);
  _instances.erase(this);
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

void Signal::scale(sample s)
{
  for (unsigned int i=0; i < Signal::size;i++)
  {
    samples[i] *= s;
  }
}

void Signal::constant(sample s)
{
  for (unsigned int i=0; i < Signal::size;i++)
  {
    samples[i] = s;
  }
}

Signal& Signal::operator=(const Signal& s)
{
  memcpy((void*) samples,(void*) s.samples,Signal::byteSize);
}

void Signal::_reset()
{
  if (samples)
    free((void*) samples);
  samples=(sample*) malloc(Signal::byteSize);
  memset((void*) samples,0,Signal::byteSize);
}

void Signal::globalConfiguration(unsigned int f,  unsigned int r)
{
  if (f>=8000)
    Signal::frequency = f;
    
  Signal::channels = 2; 
  if (r > 0)
    Signal::refreshRate = r; 
  Signal::size = Signal::channels*Signal::frequency/Signal::refreshRate;
  Signal::byteSize = sizeof(sample)*Signal::size;
  
  for (std::set<Signal*>::iterator it = _instances.begin(); it != _instances.end() ;it++)
  {
    (*it)->_reset();
  }
}



