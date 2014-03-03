#ifndef SETTINGS_
#define SETTINGS_

#include <fstream>
#include <map>
#include <iostream>
#include "variant.hpp"

Variant GetSettingsFor(std::string key);
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
