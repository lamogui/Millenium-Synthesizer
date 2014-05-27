/****************************************************************************
Nom ......... : preset.hpp
Role ........ : Classe permettant de parser des fichiers très simples pour 
                preset des instruments...
Auteur ...... : Julien DE LOOR 
Version ..... : V1.0 olol
Licence ..... : © Copydown™
****************************************************************************/


#ifndef __PRESET
#define __PRESET

#include "instrument.hpp"
#include "file.hpp"

class Preset : public AbstractFileParser {
   public:
      Preset();
      virtual ~Preset();
      
      virtual unsigned int read_from_buffer_offset(const unsigned char* buffer, 
                                                   unsigned int size,
                                                   unsigned int& offset);
                                            
                                            
      inline virtual unsigned int byte_size() const
      {
        return _size + 4;
      }
      
      //Pack instrument parameters into a preset buffer
      bool pack(AbstractInstrument* i);
      
      //Unpack and apply a preset buffer to an instrument
      bool unpack(AbstractInstrument* i);
      
      virtual unsigned int write_to_buffer_offset( unsigned char* buffer, 
                                                   unsigned int size,
                                                   unsigned int& offset) const;
      
   private:
      unsigned int _size;
      unsigned char* _buffer;
};

#endif