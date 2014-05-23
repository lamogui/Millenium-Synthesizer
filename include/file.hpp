#ifndef __FILE
#define __FILE

#include <cstdio>

class AbstractFileParser
{
   public:
      AbstractFileParser() : _buffer(0) {}
      ~AbstractFileParser() {
         if (_buffer) free(_buffer);
      }
      
      virtual unsigned int read_from_buffer(const unsigned char* buffer, 
                                            unsigned int size,
                                            unsigned int& offset) = 0;
      inline unsigned int read_from_buffer(const unsigned char* buffer, 
                                           unsigned int size)
      {
         unsigned int offset=0;
         return read_from_buffer(buffer,size,offset);
      }
      
      virtual unsigned int write_to_buffer( unsigned char* buffer, 
                                         unsigned int size,
                                         unsigned int& offset) const = 0;
      
      inline unsigned int write_to_buffer( unsigned char* buffer, 
                                           unsigned int size) const
      {
         unsigned int offset=0;
         return write_to_buffer(buffer,size,offset);
      }    
      
      virtual unsigned int size() const = 0;
      
   protected:
      unsigned char* _buffer;
};


#endif