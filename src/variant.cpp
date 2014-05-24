
/****************************************************************************
Nom ......... : variant.cpp
Role ........ : Un équivalent à QVariant plus simple
Auteur ...... : Julien DE LOOR
Version ..... : V1.0 olol
Licence ..... : © Copydown™
****************************************************************************/



#include "variant.hpp"
#include <sstream>

Variant::Variant() : _str() {}

Variant::Variant(bool value) : _str()
{
	if (value)
		_str = "true";
	else
		_str = "false";
}

Variant::Variant(int value) : _str()
{
		std::stringstream strm;
		strm << value;
		_str = strm.str();
}

Variant::Variant(unsigned int value) : _str()
{
		std::stringstream strm;
		strm << value;
		_str = strm.str();
}

Variant::Variant(float value) : _str()
{
		std::stringstream strm;
		strm << value;
		_str = strm.str();
}

Variant::Variant(std::string str) : _str(str) {}

Variant::Variant(const char* str) : _str(str) {}

Variant::Variant(char* str) : _str(str) {}

Variant::Variant(const Variant& copy) : _str(copy._str) {}

Variant& Variant::operator= (const Variant& copy)
{
		_str = copy._str;
		return *this;
}

Variant& Variant::operator= (const int& number)
{
		std::stringstream strm;
		strm << number;
		_str = strm.str();
		return *this;
}

Variant& Variant::operator= (const unsigned int &number)
{
		std::stringstream strm;
		strm << number;
		_str = strm.str();
		return *this;
}

Variant& Variant::operator= (const float& number)
{
		std::stringstream strm;
		strm << number;
		_str = strm.str();
		return *this;
}

Variant& Variant::operator= (const std::string& str)
{
	_str = str;
	return *this;
}

Variant::~Variant() {}
		
bool Variant::toBool(bool *isDefaultValue)
{
	if (_str.empty() || _str[0] == ' ')
	{
		if (isDefaultValue != NULL)
			*isDefaultValue = true;
		return false;
	}
	bool containNumber = true;
	bool containZero = true;
	for (unsigned int i = 0 ; i < _str.size(); i++)
	{
		if (containZero && (_str[i] != '0' || _str[i] != '-' || _str[i] != '.' || _str[i] != ',' || _str[i] != ' '))
			containZero = false;
		if (containNumber && ((_str[i] >= '0' && _str[i] <= '9') ||  _str[i] != '-' || _str[i] != '.' || _str[i] != ',' || _str[i] != ' '))
			containNumber = false;
		if (i < _str.size() - 3 && (_str[i] == 't' || _str[i] == 'T') && (_str[i+1] == 'r' || _str[i+1] == 'R') && (_str[i+2] == 'u' || _str[i+2] == 'U') && (_str[i+3] == 'e' || _str[i+3] == 'E'))
		{
			if (isDefaultValue != NULL)
				*isDefaultValue = false;
			return true;
		}
		else if (i < _str.size() - 4 && (_str[i] == 'f' || _str[i] == 'F') && (_str[i+1] == 'a' || _str[i+1] == 'A') && (_str[i+2] == 'l' || _str[i+2] == 'L') && (_str[i+3] == 's' || _str[i+3] == 'S') && (_str[i+4] == 'e' || _str[i+4] == 'E'))
		{
			if (isDefaultValue != NULL)
				*isDefaultValue = false;
			return false;
		}
	}
	if (containZero)
	{
		if (isDefaultValue != NULL)
			*isDefaultValue = false;
		return false;
	}
	if (containNumber)
	{
		if (isDefaultValue != NULL)
			*isDefaultValue = false;
		return true;
	}
	
	if (isDefaultValue != NULL)
		*isDefaultValue = true;
	return false;
}

int Variant::toInt(bool *isDefaultValue)
{
	int value = 0;
	bool invert = false;
	if (isDefaultValue != NULL)
		*isDefaultValue = true;
	for (unsigned int i = 0 ; i < _str.size(); i++)
	{
		if (_str[i] == '-' && value == 0)
			invert = true;
		
		switch (_str[i])
		{
			case '0':
				value *= 10;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;
			case '1':
				value *= 10;
				value += 1;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;
			case '2':
				value *= 10;
				value += 2;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;
			case '3':
				value *= 10;
				value += 3;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;	
			case '4':
				value *= 10;
				value += 4;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;	
			case '5':
				value *= 10;
				value += 5;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;	
			case '6':
				value *= 10;
				value += 6;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;	
			case '7':
				value *= 10;
				value += 7;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;	
			case '8':
				value *= 10;
				value += 8;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;
			case '9':
				value *= 10;
				value += 9;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;	
			case '.':
      case ',':
				i = static_cast<unsigned int>(_str.size());
				break;
			
			default:
				break;
		}
	}
	if (invert)
		value *= -1;
	return value;
}

unsigned int Variant::toUInt(bool *isDefaultValue)
{
	unsigned int value = 0;
	if (isDefaultValue != NULL)
		*isDefaultValue = true;
	for (unsigned int i = 0 ; i < _str.size(); i++)
	{
		switch (_str[i])
		{
			case '0':
				value *= 10;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;
			case '1':
				value *= 10;
				value += 1;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;
			case '2':
				value *= 10;
				value += 2;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;
			case '3':
				value *= 10;
				value += 3;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;	
			case '4':
				value *= 10;
				value += 4;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;	
			case '5':
				value *= 10;
				value += 5;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;	
			case '6':
				value *= 10;
				value += 6;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;	
			case '7':
				value *= 10;
				value += 7;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;	
			case '8':
				value *= 10;
				value += 8;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;
			case '9':
				value *= 10;
				value += 9;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;	
			case '.':
				i = static_cast<unsigned int>(_str.size());
				break;
			case ',':
				i = static_cast<unsigned int>(_str.size());
				break;
			default:
				break;
		}
	}
	return value;
}

float Variant::toFloat(bool *isDefaultValue)
{
	float value = 0;
	int x = 0, y = static_cast<unsigned int>(_str.size());
	bool invert = false;
	if (isDefaultValue != NULL)
		*isDefaultValue = true;
	for (unsigned int i = 0 ; i < _str.size(); i++)
	{
		if (_str[i] == '-' && value > 0)
			invert = true;

		switch (_str[i])
		{
			case '0':
				value *= 10;
				x++;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;
			case '1':
				value *= 10;
				value += 1;
				x++;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;
			case '2':
				value *= 10;
				value += 2;
				x++;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;
			case '3':
				value *= 10;
				value += 3;
				x++;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;	
			case '4':
				value *= 10;
				value += 4;
				x++;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;	
			case '5':
				value *= 10;
				value += 5;
				x++;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;	
			case '6':
				value *= 10;
				value += 6;
				x++;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;	
			case '7':
				value *= 10;
				value += 7;
				x++;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;	
			case '8':
				value *= 10;
				value += 8;
				x++;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;
			case '9':
				value *= 10;
				value += 9;
				x++;
				if (isDefaultValue != NULL)
					*isDefaultValue = false;
				break;	
			case '.':
				y = x;
				break;
			case ',':
				y = x;
				break;
			default:
				break;
		}
	}
	for (int i = 0; i < (x - y) ;i++)
		value /= 10;
	
	if (invert)
		value *= -1;
	return value;
}

std::string Variant::toString()
{
	return _str;
}
