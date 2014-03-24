
#include "enveloppe.hpp"
#include <cmath>


 void Enveloppe::step(Signal* output)
 {
    sample* samples = output->samples;

    if (_releaseTime)
    {
      //release
      for (int i=0;i < Signal::size-1;i+=2)
      {
        samples[i]=_releaseLevel*exp(-5.f*(_time-_releaseTime)/(float)release);
        samples[i+1]=samples[i];
        _time++;
      }
    }
    else
    {
      if (_time < attack)
      {
        //attack
        for (int i=0;i < Signal::size-1;i+=2)
        {
          samples[i]=1.f-exp(-5.f*_time/(float)attack);
          samples[i+1]=samples[i];
          _time++;
        }
      }
      else if (_time < attack + decay)
      {
        //decay
        const float sustain_1 = (1.f - sustain);
        for (int i=0;i < Signal::size-1;i+=2)
        {
          samples[i]=sustain + sustain_1*exp(-5.f*(_time-attack)/(float)decay);
          samples[i+1]=samples[i];
          _time++;
        }
      }
      else
      {
        //sustain
        output->constant(sustain);
        _time += Signal::size >> 1; //size/2
      }
      
      //toujours pas de release time ! on met à jour la valeur à laquel il va commencer
      _releaseLevel = output->samples[Signal::size-1]; 
    }
    
 }
