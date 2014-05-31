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
#include "interface.hpp"
#include "config.hpp"
#ifdef COMPILE_WINDOWS
#include <windows.h>
#endif

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

void SaveInstrumentPresetRoutine(AbstractInstrument* instrument);
void LoadInstrumentPresetRoutine(AbstractInstrument* _instrument);
/*
class PresetSaveButton : public AbstractButton {
  public:
    PresetSaveButton(const sf::Vector2f& size, 
                     const sf::String text, 
                     AbstractInstrument *instrument);
    PresetSaveButton(const sf::Texture &texture, 
                     const sf::IntRect &idle, 
                     const sf::IntRect &clicked,
                     AbstractInstrument *instrument);
  protected:
    virtual void clicked();   

  private:
    AbstractInstrument *_instrument;
};

class PresetLoadButton : public AbstractButton {
  public:
    PresetLoadButton(const sf::Vector2f& size, 
                     const sf::String text,
                     AbstractInstrument *instrument);
    PresetLoadButton( const sf::Texture &texture, 
                      const sf::IntRect &idle, 
                      const sf::IntRect &clicked,
                      AbstractInstrument *instrument);
  protected:
    virtual void clicked();   

  private:
    AbstractInstrument *_instrument;
};
*/
#endif
