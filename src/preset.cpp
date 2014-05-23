
#include "preset.hpp"

Preset::Preset() : 
_buffer(0),
_size(0)
{

}

Preset::~Preset() 
{
  if (_buffer) 
    free(_buffer);
}


unsigned int Preset::read_from_buffer(const unsigned char* buffer, 
                                      unsigned int size,
                                      unsigned int& offset)
{
  const unsigned int previous_offset=offset;
  if (size >= offset + 4) return 0;
  if (   buffer[offset] == 'P' && buffer[offset] == 'R'
      && buffer[offset] == 'S' && buffer[offset] == 'T')
  {
    offset+=4;
    if (size - offset)
    {
      _size = size - offset;
      _buffer=(unsigned char*)realloc((void*)_buffer,_size);
      memcpy((void*)_buffer,(void*)(buffer + offset),_size);
    }
  }
  return offset - previous_offset;
}

unsigned int Preset::write_to_buffer( unsigned char* buffer, 
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
      _buffer[_size++]=i;
      _buffer[_size++]=p->getValue() >> 8;
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
    instrument->setParameterValue( _buffer[offset++],
                                  (_buffer[offset++]<<8) | 
                                  (_buffer[offset++]));
  }
  return true;
}