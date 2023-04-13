#include "Config.h"
#include "Log.h"
#include "util/Strings.h"
#include <fstream>
#include <algorithm>

using namespace std;

namespace Crawler {

// map to store combined values
static std::map<std::string, std::string> map_all;
// maps to store new values
static std::map<std::string, std::string> map_default;
static std::map<std::string, std::string> map_user;

int Config::initCounter = 0;

bool Config::Init(){
	if(++initCounter > 1) return true;
	Log(LOG_INFO, "Config", "Initializing");
	ReadFile(TargetToPath(Target::Default), map_all);
	ReadFile(TargetToPath(Target::User), map_all);
	return true;
};

void Config::Set(const string& key, const string& value, Target target){
	if(initCounter < 1) Init();
	TargetToMap(target)[key] = value;
	map_all[key] = value;
};
void Config::Set(const string& key, int value, Target target){
	if(initCounter < 1) Init();
	TargetToMap(target)[key] = to_string(value);
	map_all[key] = to_string(value);
}
void Config::Set(const string& key, float value, Target target){
	if(initCounter < 1) Init();
	TargetToMap(target)[key] = to_string(value);
	map_all[key] = to_string(value);
}
void Config::Set(const string& key, double value, Target target){
	if(initCounter < 1) Init();
	TargetToMap(target)[key] = to_string(value);
	map_all[key] = to_string(value);
}

void Config::ReadFile(const string& filename, std::map<string, string>& map){
	if(initCounter < 1) Init();
	ifstream file(filename);
	if(!file.good()){
		Log(LOG_ERROR, "Config", "Couldn't open file: " + filename);
		file.close();
		return;
	}
	for(string line; getline(file, line); /**/){
		line = Strings::Trim(line);
		// skip empty lines
		if(line.length() == 0) continue;
		// skip comment lines
		if(Strings::StartsWith(line, "#")) continue;
		// remove comment at end of line
		vector<string> nocom = Strings::Split(line, "#");
		if(nocom.size() < 1) continue;
		// split with equals delimiter
		vector<string> split = Strings::Split(nocom[0], "=");
		// check split
		if(split.size() < 2) {
			Log(LOG_ERROR, "Config", "Invalid config line: " + line);
			continue;
		}
		string key = Strings::Trim(split[0]);
		string value = Strings::Trim(split[1]);
		if(key.length() == 0 || value.length() == 0){
			Log(LOG_ERROR, "Config", "Invalid config key/value: " + line);
			continue;
		}
		map[key] = value;
	}
	file.close();
};

void Config::WriteFile(Target target){
	if(initCounter < 1) Init();
	std::map<std::string, std::string>& newMap = TargetToMap(target);
	std::string path = TargetToPath(target);
	if(newMap.size() == 0){
		Log(LOG_INFO, "Config", "Nothing to write: " + path);
		return;
	}
	Log(LOG_INFO, "Config", "Writing config file: " + path);
	// read old app file
	map<string, string> combinedMap;
	ReadFile(path, combinedMap);
	// combine old app file with new values
	for(auto const &it: newMap){
		combinedMap[it.first] = it.second;
	}
	// open file output stream
	ofstream file;
	file.open(path.c_str());
	if(!file.good()){
		Log(LOG_ERROR, "Config", "Couldn't write config file");
		file.close();
		return;
	}
	// write new app file
	file << "# generated file" << endl;
	for(auto const &it: combinedMap){
		file << it.first << "=" << it.second << endl;
	}
	// done
	file.close();
}

string Config::GetString(const string& key, const string& defval){
	if(initCounter < 1) Init();
	auto search = map_all.find(key);
	if(search == map_all.end()){
		Log(LOG_ERROR, "Config", "Config key not set: " + key);
		return defval;
	}
	return search->second;
};
int Config::GetInt(const string& key, int defval){
	if(initCounter < 1) Init();
	string str = GetString(key, "");
	return str == "" ? defval : stoi(str);
};
bool Config::GetBool(const string& key, bool defval){
	if(initCounter < 1) Init();
	string str = GetString(key, "");
	return str == "" ? defval : (str == "0" ? false : true);
};
float Config::GetFloat(const string& key, float defval){
	if(initCounter < 1) Init();
	string str = GetString(key, "");
	return str == "" ? defval : stof(str);
};
double Config::GetDouble(const string& key, double defval){
	if(initCounter < 1) Init();
	string str = GetString(key, "");
	return str == "" ? defval : stod(str);
};

std::string Config::TargetToPath(Target target){
	if(initCounter < 1) Init();
	switch(target){
	case Target::User: return Strings::GetConfigPath("crawler.usr.cfg");
	case Target::Default: return Strings::GetConfigPath("crawler.def.cfg");
	}
    return "error";
}
std::map<std::string, std::string>& Config::TargetToMap(Target target){
	if(initCounter < 1) Init();
	switch(target){
	case Target::Runtime: return map_all;
	case Target::User: return map_user;
	case Target::Default: return map_default;
	}
	return map_all;
}

}