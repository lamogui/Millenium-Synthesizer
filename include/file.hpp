#ifndef __FILE
#define __FILE

#include <cstdio>
#include <cstdlib>

unsigned int fsize(FILE *fp);

class AbstractFileParser
{
   public:
      AbstractFileParser(); 
      virtual ~AbstractFileParser(); 
      
      virtual unsigned int read_from_buffer(const unsigned char* buffer, 
                                            unsigned int size,
                                            unsigned int& offset) = 0;
      inline unsigned int read_from_buffer(const unsigned char* buffer, 
                                           unsigned int size)
      {
         unsigned int offset=0;
         return read_from_buffer(buffer,size,offset);
      }
      
      /* Return the number of writed bytes */
      virtual unsigned int write_to_buffer( unsigned char* buffer, 
                                         unsigned int size,
                                         unsigned int& offset) const = 0;
      
      inline unsigned int write_to_buffer( unsigned char* buffer, 
                                           unsigned int size) const
      {
         unsigned int offset=0;
         return write_to_buffer(buffer,size,offset);
      }    
      
      unsigned int write_to_file(FILE* file) const;
      
      virtual unsigned int byte_size() const = 0;
      
   protected:
};


#endif