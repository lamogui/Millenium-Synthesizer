
#include "oscillator.hpp"
#include <cmath>

Oscillator::Oscillator() 
{
  frequency=&_frequency;
  amplitude=&_amplitude;
  unisson=&_unisson;
  fm=&_fm;
  shape=&_shape;
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

void Oscillator::setShape(float myshape)
{
  _shape.constant(myshape);
  shape=&_shape;
}

SinusoidalOscillator::SinusoidalOscillator()
{
}

SinusoidalOscillator::~SinusoidalOscillator()
{
}

void SinusoidalOscillator::step(Signal* output)
{
  sample* samples = output->samples;
  sample* f = frequency->samples;
  sample* a = amplitude->samples;
  sample* u = unisson->samples;
  sample* m = fm->samples;
  
  const float k=(2.0*3.1415/(float)Signal::frequency);
  for (int i=0;i < Signal::size-1;i+=2)
  {
    const float x=k*f[i]*_time;
    samples[i] = a[i]*sin(x + 10.0*m[i]);
    samples[i+1] = samples[i];
    _time++;
  }
}

SquareOscillator::SquareOscillator()
{
  setShape(0.5);
}

SquareOscillator::~SquareOscillator()
{
}


void SquareOscillator::step(Signal* output)
{
  float t;
  sample* samples = output->samples;
  sample* f = frequency->samples;
  sample* a = amplitude->samples;
  sample* u = unisson->samples;
  sample* m = fm->samples;
  sample* s = shape->samples;
  
  for (int i=0;i < Signal::size-1;i+=2)
  {
    //std::cout << "t=" << t << std::endl;
    t=fmod(_time/(float)Signal::frequency,1.0/f[i])*f[i];
    if (t>s[i]) {
      samples[i]=a[i];
    }
    else {
      samples[i]=-a[i];
    }
    samples[i+1] = -samples[i];
    _time++;
  }
}

