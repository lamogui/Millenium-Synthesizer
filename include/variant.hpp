
/****************************************************************************
Nom ......... : variant.hpp
Role ........ : Un équivalent à QVariant plus simple
Auteur ...... : Julien DE LOOR
Version ..... : V1.0 olol
Licence ..... : © Copydown™
****************************************************************************/


#ifndef VARIANT___
#define VARIANT___

#include <string>

class Variant
{
	public:
		Variant();
		Variant(bool value);
		Variant(int value);
		Variant(unsigned int value);
		Variant(float value);
		Variant(std::string str);
		Variant(const char* str);
		Variant(char* str);
		Variant(const Variant& copy);
		Variant& operator= (const Variant& copy);
		Variant& operator= (const int& number);
		Variant& operator= (const unsigned int &number);
		Variant& operator= (const float& number);
		Variant& operator= (const std::string& str);
		virtual ~Variant();
		
		bool toBool(bool *isDefaultValue = NULL);
		int toInt(bool *isDefaultValue = NULL);
		unsigned int toUInt(bool *isDefaultValue = NULL);
		float toFloat(bool *isDefaultValue = NULL);
		std::string toString();
		
	private:
		std::string _str;

};

#endif
