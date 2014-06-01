/****************************************************************************
Nom ......... : signal.cpp
Role ........ : Implémente une classe gérant une unité de son (classe Signal) 
                Ainsi que des modeles abstrait pour le générer ce son...
Auteur ...... : Julien DE LOOR & Nadher ALI
Version ..... : V1.7 olol
Licence ..... : © Copydown™
****************************************************************************/

#include "signal.hpp"

unsigned int Signal::frequency=44100;
unsigned int Signal::refreshRate=50;
unsigned int Signal::size=Signal::frequency/Signal::refreshRate;
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

void Signal::addOffset(sample offset)
{
  for (unsigned int i=0; i < Signal::size;i++)
  {
    samples[i] += offset;
  }
}

void Signal::scale(sample scale)
{
  for (unsigned int i=0; i < Signal::size;i++)
  {
    samples[i] *= scale;
  }
}

void Signal::constant(sample constant)
{
  for (unsigned int i=0; i < Signal::size;i++)
  {
    samples[i] = constant;
  }
}

Signal& Signal::operator=(const Signal& s)
{
  memcpy((void*) samples,(void*) s.samples,Signal::byteSize);
  return *this;
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

  if (r > 0)
    Signal::refreshRate = r; 
  Signal::size = Signal::frequency/Signal::refreshRate;
  Signal::byteSize = sizeof(sample)*Signal::size;
  
  for (std::set<Signal*>::iterator it = _instances.begin(); it != _instances.end() ;it++)
  {
    (*it)->_reset();
  }
}

void Signal::saturate(sample min, sample max)
{
  for (unsigned int i=0; i < Signal::size;i++)
  {
    if (samples[i] > max) samples[i] = max;
    else if (samples[i] < min) samples[i] = min;
  }
}

double Signal::energy()
{
  double energy=0;
  for (unsigned int i=0; i < Signal::size;i++)
  {
    energy += samples[i]*samples[i];
  }
  return energy;
}

void Signal::tfd(Signal& out_tfd)
{
  out_tfd.reset();
  for (unsigned int k=0; k < Signal::size;k++)
  {
    for (unsigned int n=0; n < Signal::size;n++)
    {
      double a=2*3.14159*k*n/(double)(Signal::size);
      double cos_a=cos(a);
      double sin_a=sin(a);
      out_tfd.samples[k] += abs(samples[n])*(sqrt(abs(cos_a*cos_a+sin_a*sin_a)));  
    }
  }
}


