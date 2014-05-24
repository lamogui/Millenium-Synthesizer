/****************************************************************************
Nom ......... : settings.hpp
Role ........ : Definie un clone de QSettings pour lire des fichiers ini
Auteur ...... : Julien DE LOOR 
Version ..... : V1.0 olol
Licence ..... : © Copydown™
****************************************************************************/


#ifndef SETTINGS_
#define SETTINGS_

#include <fstream>
#include <map>
#include <iostream>
#include "variant.hpp"

bool GetSettingsFor(std::string key, bool defValue);
unsigned int GetSettingsFor(std::string key, unsigned int defValue);
int GetSettingsFor(std::string key, int defValue);
float GetSettingsFor(std::string key, float defValue);
std::string GetSettingsFor(std::string key, const std::string& defValue);

void SetSettings(std::string key,const Variant &yourvalue);

class Settings 
{
	public :
		bool loadFile(const char* filename);
		void setValue(std::string key,const Variant &yourvalue);
		Variant value(std::string key);
	    static Settings& getInstance();
	
	private :
		Settings& operator= (const Settings&){ return *this; }
		Settings (const Settings&){}
		Settings();
		~Settings();
	
		static Settings _instance;
		void saveFile();
		
		std::map<std::string,Variant> _map;
		std::string _filename;
};

#endif
