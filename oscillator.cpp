
#include "oscillator.hpp"
#include <cmath>

Oscillator::Oscillator() : _dt(0)
{
  frequency=&_frequency;
  amplitude=&_amplitude;
  unisson=&_unisson;
  fm=&_fm;
}

Oscillator::~Oscillator()
{

}

void Oscillator::setFrequency(float f)
{
  _frequency.constant(f);
  frequency=&_frequency;
}

void Oscillator::setAmplitude(float a)
{
  _amplitude.constant(a);
  amplitude=&_amplitude;
}

void Oscillator::setUnisson(float u)
{
  _unisson.constant(u);
  unisson=&_unisson;
}

void Oscillator::setFM(float myfm)
{
  _fm.constant(myfm);
  fm=&_fm;
}

SinusoidalOscillator::SinusoidalOscillator()
{
}

SinusoidalOscillator::~SinusoidalOscillator()
{
}

void SinusoidalOscillator::stepfill(Signal* output)
{
  sample* samples = output->samples;
  sample* f = frequency->samples;
  sample* a = amplitude->samples;
  sample* u = unisson->samples;
  sample* m = fm->samples;
  
  const float k=(2.0*3.1415/(float)Signal::frequency);
  for (int i=0;i < Signal::size-1;i+=2)
  {
    const float x=k*f[i]*_dt;
    samples[i] = (a[i]/6.0)*(sin(x + m[i])+u[i]*sin(2.0*x + m[i])+ 0.5*u[i]*sin(4.0*x + m[i])+0.25*u[i]*sin(8.0*x + m[i]) + 0.5*u[i]*sin(0.5*x + m[i]) + 0.25*u[i]*sin(0.25*x + m[i]));
    samples[i+1] = samples[i];
    _dt++;
  }
}