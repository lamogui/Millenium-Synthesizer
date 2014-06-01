/****************************************************************************
Nom ......... : file.hpp
Role ........ : Des fonctions et classes utiles pour manipuler facilement des 
                fichiers
Auteur ...... : Julien DE LOOR
Version ..... : V1.7 olol
Licence ..... : © Copydown™
****************************************************************************/

#ifndef __FILE___LOL
#define __FILE___LOL

#include <cstdio>
#include <cstdlib>
#include <iostream>

unsigned int fsize(FILE *fp);

class AbstractFileParser
{
   public:
      AbstractFileParser(); 
      virtual ~AbstractFileParser(); 
      
      inline unsigned int read_from_buffer(const unsigned char* buffer, 
                                           unsigned int size) {
         unsigned int offset=0;
         std::cout << "Call  AbstractFileParser " << size << std::endl;
         return this->read_from_buffer_offset(buffer,size,offset);
      }
      
      virtual unsigned int read_from_buffer_offset(const unsigned char* buffer, 
                                                   unsigned int size,
                                                   unsigned int& offset) = 0;
      
      
      /* Return the number of writed bytes */
      virtual unsigned int write_to_buffer_offset( unsigned char* buffer, 
                                                   unsigned int size,
                                                   unsigned int& offset) 
                                                   const = 0;
      
      inline unsigned int write_to_buffer( unsigned char* buffer, 
                                           unsigned int size) const
      {
         unsigned int offset=0;
         return write_to_buffer_offset(buffer,size,offset);
      }    
      
      unsigned int write_to_file(FILE* file) const;
      
      virtual unsigned int byte_size() const = 0;
      
   protected:
};


#endif
