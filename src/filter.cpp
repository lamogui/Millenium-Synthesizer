#include "filter.hpp"
#include <iostream>

Filter::Filter()
{
}

Filter::Filter(float f, float m)
{
  setFrequency(f);
  setResonance(m);
}

Filter::~Filter()
{
}

void Filter::setFrequency(float f)
{
  _frequency.constant(f);
}

void Filter::setResonance(float m)
{
  _resonance.constant(m);
}

LowPassFilter::LowPassFilter() : Filter(), _y_1(0) {}
LowPassFilter::LowPassFilter(float f, float m) : Filter(f,m), _y_1(0) {}
LowPassFilter::~LowPassFilter() {}
void LowPassFilter::step(Signal* inout)
{
  sample* samples = inout->samples;
  sample* f = getFrequency().samples;
  const float pi_2 = 3.1415f*2.f;
  const float te = 1.f/(float)Signal::frequency;
  for (unsigned int i=0;i < Signal::size;i++)
  {
    const float w0=f[i]*pi_2;
    const float w0te=w0*te;
    const float d=1.f+w0te;
    const float a0=w0te/d;
    const float b1=1.f/d;
    
    _y_1 = samples[i] = a0*samples[i]+b1*_y_1;
  }
}

LowPassFilter2::LowPassFilter2() : Filter(), _y_1(0), _y_2(0) {}
LowPassFilter2::LowPassFilter2(float f, float m) : Filter(f,m), _y_1(0), _y_2(0){}
LowPassFilter2::~LowPassFilter2() {}
void LowPassFilter2::step(Signal* inout)
{
sample* samples = inout->samples;
  sample* f = getFrequency().samples;
  sample* m = getResonance().samples;
  const float pi_2 = 3.1415f*2.f;
  const float te = 1.f/(float)Signal::frequency;
  const float _2te=te*2.f;
  for (unsigned int i=0; i<Signal::size;i++)
  {
    const float w0=f[i]*pi_2;
    const float w0te=w0*te;
    const float w02te=w0*_2te;
    const float w02te2=w0te*w0te;
    const float _2mw0te=m[i]*w02te;
    float a=w02te2+_2mw0te+1.f;
    a=1.f/a;
    const float b=_2mw0te+2.f;
    samples[i]=a*(_y_1*b-_y_2+w02te2*samples[i]);
    //std::cout << samples[i+1]<<std::endl;
    _y_2=_y_1;
    _y_1=samples[i];
  }
}

BandPassFilter2::BandPassFilter2() : Filter(), _y_1(0), _y_2(0), _x_1(0) {}
BandPassFilter2::BandPassFilter2(float f, float m) : Filter(f,m), _y_1(0), _y_2(0), _x_1(0) {}
BandPassFilter2::~BandPassFilter2() {}
void BandPassFilter2::step(Signal* inout)
{
  sample* samples = inout->samples;
  sample* f = getFrequency().samples;
  sample* m = getResonance().samples;
  const float pi_2 = 3.1415f*2.f;
  const float te = 1.f/(float)Signal::frequency;
  for (unsigned int i=0;i < Signal::size;i++)
  {
    const float w0=f[i]*pi_2;
    const float w0te=w0*te;
    const float mw0te=w0te*2.f*m[i];
    const float w0te2=w0te*w0te;
    const float d=1.f+mw0te+w0te2;
    const float a0=mw0te/d;
    const float b1=(2.f + mw0te)/d;
    const float b2=1.f/d;
    const float save_x_1 = samples[i];
    const float k=1.f/0.7f;
    samples[i] = k*a0*samples[i]-k*a0*_x_1+b1*_y_1-b2*_y_2;
    _y_2 = _y_1;
    _y_1 = samples[i];
    _x_1 = save_x_1;
  }
}

HighPassFilter::HighPassFilter() : Filter(), _y_1(0), _x_1(0) {}
HighPassFilter::HighPassFilter(float f, float m) : Filter(f, m), _y_1(0), _x_1(0) {}
HighPassFilter::~HighPassFilter() {}
void HighPassFilter::step(Signal* inout)
{
  sample* samples = inout->samples;
  sample* f = getFrequency().samples;
  const float pi_2 = 3.1415f*2.f;
  const float te = 1.f/(float)Signal::frequency;
  for (unsigned int i=0; i<Signal::size;i++)
  {
    const float w0=f[i]*pi_2;
    const float w0te=w0*te;
    float d=1.f+w0te;
    d=1.f/d;
    const float save_x_1=samples[i];
    samples[i]=_y_1*d+samples[i]*d-_x_1*d;
    _y_1= samples[i];
    _x_1=save_x_1;
  }
}

HighPassFilter2::HighPassFilter2() : Filter(), _y_1(0), _y_2(0), _x_1(0), _x_2(0) {}
HighPassFilter2::HighPassFilter2(float f, float m) : Filter(f, m), _y_1(0), _y_2(0), _x_1(0), _x_2(0) {}
HighPassFilter2::~HighPassFilter2() {}
void HighPassFilter2::step(Signal* inout)
{
  sample* samples = inout->samples;
  sample* f = getFrequency().samples;
  sample* m = getResonance().samples;
  const float pi_2 = 3.1415f*2.f;
  const float te = 1.f/(float)Signal::frequency;
  const float _2te=te*2.f;
  for (unsigned int i=0; i<Signal::size;i++)
  {
    const float w0=f[i]*pi_2;
    const float w0te=w0*te;
    const float w02te=w0*_2te;
    const float w02te2=w0te*w0te;
    const float _2mw0te=m[i]*w02te;
    float a=w02te2+_2mw0te+1.f;
    a=1.f/a;
    const float b=_2mw0te+2.f;
    const float _2_x_1=2*_x_1;
    const float save_x_1=samples[i];
    samples[i]=a*(_y_1*b-_y_2+samples[i]-_2_x_1+_x_2);
    //std::cout << samples[i]<<std::endl;
    _y_2=_y_1;
    _y_1=samples[i];
    _x_2=_x_1;
    _x_1=save_x_1;
  }
}

Rejector::Rejector() : Filter(), _y_1(0), _y_2(0), _x_1(0), _x_2(0) {}
Rejector::Rejector(float f, float m) : Filter(f, m), _y_1(0), _y_2(0), _x_1(0), _x_2(0) {}
Rejector::~Rejector() {}
void Rejector::step(Signal* inout) {
  sample* samples = inout->samples;
  sample* f = getFrequency().samples;
  sample* m = getResonance().samples;
  const float te = 1.f/(float)Signal::frequency;
  for (unsigned int i=0; i<Signal::size; i++)
  {
    const float w0te=f[i]*te;
    const float w02te2=w0te*w0te;
    const float mw0te=m[i]*w0te;
    const float a=1.f/(w02te2+2.f*mw0te+1.f);
    const float b=2.f*mw0te+2.f;
    const float c=w02te2+1.f;
    const float save_x_1=samples[i];
    samples[i]=a*(_y_1*b-_y_2+samples[i]*c-2.f*_x_1+_x_2);
    _y_2=_y_1;
    _y_1=samples[i];
    _x_2=_x_1;
    _x_1=save_x_1;
  }
}


