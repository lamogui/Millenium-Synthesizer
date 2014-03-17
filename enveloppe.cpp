
#include "enveloppe.hpp"


 void Enveloppe::step(Signal* output)
 {
    if (_releaseTime)
    {
      //release
      for (int i=0;i < Signal::size-1;i+=2)
      {

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
      _releaseLevel = output[Signal::size-1]; 
    }
    
 }