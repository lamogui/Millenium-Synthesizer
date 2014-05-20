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

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr

void Signal::fft(Signal& out_fft)
{
  //variables for the fft
	unsigned long n,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta,tempr,tempi;

	//the complex array is real+complex so the array 
    //as a size n = 2* number of complex samples
    //real part is the data[index] and 
    //the complex part is the data[index+1]

	//new complex array of size n=2*sample_rate

	float* vector=new float [2*Signal::frequency];

	//put the real array in a complex array
	//the complex part is filled with 0's
	//the remaining vector with no data is filled with 0's
	for(n=0; n<Signal::frequency;n++)
	{
		if(n<Signal::size)
			vector[2*n]=samples[n];
		else
			vector[2*n]=0;
		vector[2*n+1]=0;
	}

	//binary inversion (note that the indexes 
    //start from 0 witch means that the
    //real part of the complex is on the even-indexes 
    //and the complex part is on the odd-indexes)
	n=Signal::frequency << 1;
	j=0;
	for (i=0;i<n/2;i+=2) {
		if (j > i) {
			SWAP(vector[j],vector[i]);
			SWAP(vector[j+1],vector[i+1]);
			if((j/2)<(n/4)){
				SWAP(vector[(n-(i+2))],vector[(n-(j+2))]);
				SWAP(vector[(n-(i+2))+1],vector[(n-(j+2))+1]);
			}
		}
		m=n >> 1;
		while (m >= 2 && j >= m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	//end of the bit-reversed order algorithm

	//Danielson-Lanzcos routine
	mmax=2;
	while (n > mmax) {
		istep=mmax << 1;
		theta=(2*3.14159/mmax);//*sign;
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j=i+mmax;
				tempr=wr*vector[j-1]-wi*vector[j];
				tempi=wr*vector[j]+wi*vector[j-1];
				vector[j-1]=vector[i-1]-tempr;
				vector[j]=vector[i]-tempi;
				vector[i-1] += tempr;
				vector[i] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
  
  delete vector;
	//end of the algorithm
	/*
	//determine the fundamental frequency
	//look for the maximum absolute value in the complex array
	fundamental_frequency=0;
	for(i=2; i<=sample_rate; i+=2)
	{
		if((pow(vector[i],2)+pow(vector[i+1],2))>(pow(vector[fundamental_frequency],2)+pow(vector[fundamental_frequency+1],2))){
			fundamental_frequency=i;
		}
	}

	//since the array of complex has the format [real][complex]=>[absolute value]
	//the maximum absolute value must be ajusted to half
	fundamental_frequency=(long)floor((float)fundamental_frequency/2);
*/
}




