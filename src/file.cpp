/****************************************************************************
Nom ......... : file.cpp
Role ........ : Des fonctions et classes utiles pour manipuler facilement des 
                fichiers
Auteur ...... : Julien DE LOOR
Version ..... : V1.0 olol
Licence ..... : © Copydown™
****************************************************************************/

#include "file.hpp"

 unsigned int fsize(FILE *fp){
     unsigned int prev=ftell(fp);
    fseek(fp, 0L, SEEK_END);
    unsigned int sz=ftell(fp);
    fseek(fp,prev,SEEK_SET); //go back to where we were
    return sz;
}

AbstractFileParser::AbstractFileParser() 
{

}

AbstractFileParser::~AbstractFileParser()
{

}

unsigned int AbstractFileParser::write_to_file(FILE* file) const
{
  unsigned int target_size=byte_size();
  unsigned int real_size;
  if (!target_size) return 0;
  unsigned char* buffer = (unsigned char*) malloc(target_size);
  if (real_size=write_to_buffer(buffer,target_size))
  {
    if (real_size!=fwrite((void*)buffer,1,real_size,file))
    {
      printf("fwrite error !\n"); 
      free((void*)buffer);
      return 0;
    }
  }
  else 
  {
    free((void*)buffer);
    return 0;
  }
  free((void*)buffer);
  return real_size;
}
