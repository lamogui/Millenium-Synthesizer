/****************************************************************************
Nom ......... : preset.cpp
Role ........ : Classe permettant de parser des fichiers très simples pour 
                preset des instruments...
Auteur ...... : Julien DE LOOR 
Version ..... : V1.0 olol
Licence ..... : © Copydown™
****************************************************************************/

#include "preset.hpp"

Preset::Preset() : 
AbstractFileParser(),
_buffer(0),
_size(0)
{

}

Preset::~Preset() 
{
  if (_buffer) 
    free(_buffer);
}


unsigned int Preset::read_from_buffer_offset(const unsigned char* buffer, 
                                             unsigned int size,
                                             unsigned int& offset)
{
  const unsigned int previous_offset=offset;
  if (size <= offset + 4) return 0;
  
  std::cout << "try Load PRST file" << std::endl;
  if (   buffer[offset] == 'P' && buffer[offset+1] == 'R'
      && buffer[offset+2] == 'S' && buffer[offset+3] == 'T')
  {
    offset+=4;
    std::cout << "Load PRST file" << std::endl;
    if (size - offset)
    {
      _size = size - offset;
      _buffer=(unsigned char*)realloc((void*)_buffer,_size);
      memcpy((void*)_buffer,(void*)(buffer + offset),_size);
    }
  }
  return offset - previous_offset;
}

unsigned int Preset::write_to_buffer_offset( unsigned char* buffer, 
                                             unsigned int size,
                                             unsigned int& offset) const
{
  if (size < offset + byte_size()) return 0;
  buffer[offset++]='P';
  buffer[offset++]='R';
  buffer[offset++]='S';
  buffer[offset++]='T';
  memcpy((void*)(buffer + offset),(void*)_buffer,_size);
  offset+=_size;
  return size+4;
}

bool Preset::pack(AbstractInstrument* instrument)
{
  if (!instrument) return false;
  _buffer=(unsigned char*)realloc((void*)_buffer,3*255); //1 BYTE + 1 WORD
  _size=0;
  
  for (unsigned int i=0; i < 255; i++)
  {
    InstrumentParameter* p;
    if (p=instrument->getParameter(i)) {
      //std::cout << "save param " <<  i << " value " << p->getValue() << std::endl;
      _buffer[_size++]=i;
      _buffer[_size++]=(p->getValue() >> 8) & 0xFF;
      _buffer[_size++]=p->getValue() & 0xFF;
    }
  }
  return true;
}

bool Preset::unpack(AbstractInstrument* instrument)
{
  if (!_buffer ||!_size || !instrument) return false;
  unsigned offset=0;
  for (unsigned int i=0; i < _size/3; i++)
  {
    unsigned char id = _buffer[offset++];
    unsigned int p1 = _buffer[offset++];
    unsigned int p2 = _buffer[offset++];
    short param = (p1 << 8) | p2;
    //std::cout << "Loading param " << (unsigned int) id << " value " << param  << " " << p1  <<  " " << p2 << std::endl;
    bool lol = instrument->setParameterValue( id, param );
    //std::cout << "return " << lol << std::endl;
  }
  return true;
}