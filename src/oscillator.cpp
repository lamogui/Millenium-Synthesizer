
#include "oscillator.hpp"
#include <cmath>
#include <random>

Oscillator::Oscillator() 
{
  /*frequency=&_frequency;
  amplitude=&_amplitude;
  unisson=&_unisson;
  fm=&_fm;
  shape=&_shape;*/
}

Oscillator::~Oscillator()
{

}

void Oscillator::setFrequency(float f)
{
  _frequency.constant(f);
  //frequency=&_frequency;
}

void Oscillator::setAmplitude(float a)
{
  _amplitude.constant(a);
  //amplitude=&_amplitude;
}

void Oscillator::setUnisson(float u)
{
  _unisson.constant(u);
  //unisson=&_unisson;
}

void Oscillator::setFM(float myfm)
{
  _fm.constant(myfm);
  //fm=&_fm;
}

void Oscillator::setShape(float myshape)
{
  _shape.constant(myshape);
  //shape=&_shape;
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
  sample* f = getFrequency().samples;
  sample* a = getAmplitude().samples;
  sample* u = getUnisson().samples;
  sample* m = getFM().samples;
  
  const float k=(2.0*3.1415/(float)Signal::frequency);
  for (unsigned int i=0;i < Signal::size;i++)
  {
    const float x=k*f[i]*_time;
    samples[i] = a[i]*sin(x + m[i]);
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
  sample* f = getFrequency().samples;
  sample* a = getAmplitude().samples;
  sample* u = getUnisson().samples;
  sample* m = getFM().samples;
  sample* s = getShape().samples;
  
  for (unsigned int i=0;i < Signal::size;i++)
  {
    t=fmod(_time/(float)Signal::frequency + m[i],1.0/f[i])*f[i];
    if (t>s[i]) {
      samples[i]=a[i];
    }
    else {
      samples[i]=-a[i];
    }
    _time++;
  }
}

SawOscillator::SawOscillator()
{
}

SawOscillator::~SawOscillator()
{
}


void SawOscillator::step(Signal* output)
{
  sample* samples = output->samples;
  sample* f = getFrequency().samples;
  sample* a = getAmplitude().samples;
  sample* u = getUnisson().samples;
  sample* m = getFM().samples;
  sample* s = getShape().samples;
  
  for (unsigned int i=0;i < Signal::size;i++)
  {
    samples[i]=-a[i]*(2.f*f[i]*fmod(_time/(float)Signal::frequency + m[i],1.f/f[i])-1.f);
    _time++;
  }
}

TriangleOscillator::TriangleOscillator()
{
}

TriangleOscillator::~TriangleOscillator()
{
}


void TriangleOscillator::step(Signal* output)
{
  float t;
  sample* samples = output->samples;
  sample* f = getFrequency().samples;
  sample* a = getAmplitude().samples;
  sample* u = getUnisson().samples;
  sample* m = getFM().samples;
  sample* s = getShape().samples;
  
  for (unsigned int i=0;i < Signal::size;i++)
  {
    t=(1.f+2.f*s[i])*(fabs(4.f*f[i]*fmod(_time/(float)Signal::frequency + m[i],1.f/f[i])-2.f)-(1.f));
    /*if (t > 1.f - 0.5f*s[i]) t=1.f - 0.5f*s[i];
    else if (t < -1.f + 0.5f*s[i]) t=-1.f + 0.5f*s[i];*/
    if (t > 1.f)  t=1.f;
    else if (t < -1.f)  t=-1.f;
    samples[i]=a[i]*t;
    _time++;
  }
}

WhiteNoiseOscillator::WhiteNoiseOscillator()
{
}

WhiteNoiseOscillator::~WhiteNoiseOscillator()
{
}

void WhiteNoiseOscillator::step(Signal* output)
{
  sample* samples = output->samples;
  std::default_random_engine generator;
  std::uniform_real_distribution<float> distribution(-1.0,1.0); 

  for (unsigned int i=0;i < Signal::size;i++)
  {
    samples[i]= distribution(generator);
    _time++;
  }
}


