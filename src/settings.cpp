
/****************************************************************************
Nom ......... : settings.cpp
Role ........ : Definie un clone de QSettings pour lire des fichiers ini
Auteur ...... : Julien DE LOOR 
Version ..... : V1.0 
Licence ..... : © Copydown™
****************************************************************************/

#include "settings.hpp"

Settings Settings::_instance = Settings();

Settings::Settings() {}

Settings::~Settings() {}


Settings& Settings::getInstance()
{
	return _instance;
}

void Settings::saveFile()
{
	if (_filename.empty())
	{
		std::cerr << "failled to save the file no filename" << std::endl;
		return; 
	}
	std::ofstream strm(_filename.c_str(), std::ofstream::binary);
	if (!strm.is_open())
	{
		std::cerr << "cannot write on file : "  << _filename << std::endl;
		return; 
	}
	std::string head;
	for (std::map<std::string,Variant>::iterator it = _map.begin(); it != _map.end(); it++)
	{
		std::string newHead;
		if (it->first.find('/') != std::string::npos)
		{
			newHead = it->first;
			newHead.erase(newHead.find_first_of('/'));
			newHead = '[' + newHead + ']';
		}
		else 
			newHead = "[General]";
			
		if (head != newHead && it == _map.begin())
			head = newHead, strm << head << "\r\n\r\n";
		else if (head != newHead)
			head = newHead, strm << "\r\n" << head << "\r\n";
		std::string copyfit = it->first;
		copyfit.erase(0,copyfit.find('/') + 1);
		strm << copyfit << " = ";
		if (it->second.toString().find(' ') != std::string::npos)
			strm << "\"" << it->second.toString() << "\";\r\n";
		else
			strm << it->second.toString() << ";\r\n";
	}
}

bool Settings::loadFile(const char* filename)
{
	if (filename == NULL || filename[0] == '\0')
	{
		std::cerr << "falled to read the file : no filename" << std::endl;
		return false; 
	}
	_filename = filename;
	std::ifstream strm(_filename.c_str(), std::ifstream::binary);
	if (!strm.is_open())
	{
		std::cerr << "cannot open file : "  << _filename << std::endl;
		return false; 
	}
	
	std::string section = "General", currentKey, fileString, currentVariant;
	bool ignoreSpaces = true,isInSection = false, isInKey = true, isInValue = false;
	
	while (!strm.eof() && !strm.bad())
	{
		char currentChar;
		strm.get(currentChar);
		fileString += currentChar;
	}
	strm.close();
	for (unsigned int i = 0; i < fileString.size(); i++)
	{
		switch (fileString[i]) 
		{
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '-':
			case '_':
			case '\'':
			case '/':
			case '\\':
			case ',':
			case '.':
			case '!':
			case '?':
			case '(':
			case ')':
				if (isInSection)
					section += fileString[i];
				else if (isInKey)
					currentKey += fileString[i];
				else if (isInValue)
					currentVariant += fileString[i];
				break;
			case ' ':
				if (ignoreSpaces)
					break;
				if (isInSection)
					section += fileString[i];
				else if (isInKey)
					currentKey += fileString[i];
				else if (isInValue)
					currentVariant += fileString[i];
				break;
			case '[':
				if (i == 0 || (i > 0 && fileString[i-1] == '\n')) 
				{
					ignoreSpaces = true;
					isInValue = false;
					isInKey = false;
					isInSection = true;
					if (!currentKey.empty() && !section.empty() && !currentVariant.empty())
						_map[section + '/' + currentKey] = Variant(currentVariant);
					section = "";
					currentKey = "";
					currentVariant = "";
				}
				else 
				{
					if (isInSection)
						section += fileString[i];
					else if (isInKey)
						currentKey += fileString[i];
					else if (isInValue)
						currentVariant += fileString[i];
				}
				break;
			case ']':
				if (isInSection) {
					isInValue = false;
					isInKey = true;
					isInSection = false;
				}
				else
				{
					if (isInSection)
						section += fileString[i];
					else if (isInKey)
						currentKey += fileString[i];
					else if (isInValue)
						currentVariant += fileString[i];
				}
				break;
			case '\"':
				if (!isInValue)
					break;
				if (ignoreSpaces)
					ignoreSpaces = false;
				else
					ignoreSpaces = true;
				break;
			case '=':
				if (isInKey)
				{
					isInValue = true;
					isInKey = false;
					isInSection = false;
				}
				break;
			case ';':
				if (isInValue)
				{
					isInValue = false;
					isInKey = true;
					isInSection = false;
					if (!currentKey.empty() && !section.empty() && !currentVariant.empty())
						_map[section + '/' + currentKey] = Variant(currentVariant);
					currentKey = "";
					currentVariant = "";
				}
				break;
			default:
				break;
		}
	}
	return true;
}

void Settings::setValue(std::string key, const Variant &yourvalue)
{
	if (key.find('/') == std::string::npos)
		key = "General/" + key;
	_map[key] = yourvalue;
	saveFile();
}

Variant Settings::value(std::string key)
{
	if (_map.find(key) != _map.end())
		return _map.find(key)->second;
	else if (_map.find("General/" + key) != _map.end())
		return _map.find("General/" + key)->second;
	return Variant();
}

bool GetSettingsFor(std::string key, bool defValue)
{
  bool defaultValue=true;
  bool v = Settings::getInstance().value(key).toBool(&defaultValue);
  if (defaultValue)
  {
    Settings::getInstance().setValue(key,Variant(defValue));
    return defValue;
  }
	return v;
}

unsigned int GetSettingsFor(std::string key, unsigned int defValue)
{
  bool defaultValue=true;
  unsigned int v = Settings::getInstance().value(key).toUInt(&defaultValue);
  if (defaultValue)
  {
    Settings::getInstance().setValue(key,Variant(defValue));
    return defValue;
  }
	return v;
}

int GetSettingsFor(std::string key, int defValue)
{
  bool defaultValue=true;
  int v = Settings::getInstance().value(key).toInt(&defaultValue);
  if (defaultValue)
  {
    Settings::getInstance().setValue(key,Variant(defValue));
    return defValue;
  }
	return v;
}

float GetSettingsFor(std::string key, float defValue)
{
  bool defaultValue=true;
  float v = Settings::getInstance().value(key).toFloat(&defaultValue);
  if (defaultValue)
  {
    Settings::getInstance().setValue(key,Variant(defValue));
    return defValue;
  }
	return v;
}

std::string GetSettingsFor(std::string key, const std::string& defValue)
{
  std::string v = Settings::getInstance().value(key).toString();
  if (v.empty())
  {
    Settings::getInstance().setValue(key,Variant(defValue));
    return defValue;
  }
	return v;
}

void SetSettings(std::string key,const Variant &yourvalue)
{
  
	Settings::getInstance().setValue(key,yourvalue);
}
