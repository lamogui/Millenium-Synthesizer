
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
        samples[i]=1-exp(-5.f*_time/(float)attack);
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

          _time++;
        }
      }
      else if (_time < attack + decay)
      {
        //decay
        for (int i=0;i < Signal::size-1;i+=2)
        {

          _time++;
        }
      }
      else
      {
        for (int i=0;i < Signal::size-1;i+=2)
        {

          _time++;
        }
        //sustain
      }
      
      //toujours pas de release time ! on met à jour la valeur à laquel il va commencer
      _releaseLevel = output->samples[Signal::size-1]; 
    }
    
 }
