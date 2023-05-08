#include "Config.h"
#include "Log.h"
#include "util/Strings.h"
#include <fstream>
#include <algorithm>

using namespace std;

namespace Crawler {

// map to store combined values
static map<string, map<string, string>> files;

int Config::initCounter = 0;

bool Config::Init(){
	if(++initCounter > 1) return true;
	Log(LOG_INFO, "Config", "Initializing");
	return true;
};

void Config::Set(const string& file, const string& key, const string& value){
	if(initCounter < 1) Init();
	files[file][key] = value;
};
void Config::Set(const string& file, const string& key, int value){
	if(initCounter < 1) Init();
	files[file][key] = to_string(value);
}
void Config::Set(const string& file, const string& key, float value){
	if(initCounter < 1) Init();
	files[file][key] = to_string(value);
}
void Config::Set(const string& file, const string& key, double value){
	if(initCounter < 1) Init();
	files[file][key] = to_string(value);
}

void Config::ReadFile(const string& filename){
	if(initCounter < 1) Init();
	string path = Strings::GetConfigPath(filename + ".cfg");
	ifstream file(path);
	LogInfo("Config", iLog << "Reading file: " << path);
	if(!file.good()){
		Log(LOG_ERROR, "Config", "Couldn't open file: " + path);
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
		// LogDebug("Config", iLog << filename << "[" << key << "] = " << value);
		files[filename][key] = value;
	}
	file.close();
};

void Config::WriteFile(const string& filename){
	if(initCounter < 1) Init();
	string path = Strings::GetConfigPath(filename + ".cfg");
	LogInfo("Config", iLog << "Writing file: " << path);
	auto& fileMap = files[filename];
	if(fileMap.size() == 0){
		Log(LOG_INFO, "Config", "Nothing to write: " + path);
		return;
	}
	Log(LOG_INFO, "Config", "Writing config file: " + path);
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
	for(auto const &it: fileMap){
		file << it.first << "=" << it.second << endl;
	}
	// done
	file.close();
}

string Config::GetString(const string& file, const string& key, const string& defval){
	if(initCounter < 1) Init();
	if(!files.contains(file) || !files[file].contains(key)){
		LogError("Config", iLog << "Config key not set! file=" << file << " key=" << key);
		return defval;
	}
	return files[file][key];
};
int Config::GetInt(const string& file, const string& key, int defval){
	if(initCounter < 1) Init();
	string str = GetString(file, key, "");
	return str == "" ? defval : stoi(str);
};
bool Config::GetBool(const string& file, const string& key, bool defval){
	if(initCounter < 1) Init();
	string str = GetString(file, key, "");
	return str == "" ? defval : (str == "0" ? false : true);
};
float Config::GetFloat(const string& file, const string& key, float defval){
	if(initCounter < 1) Init();
	string str = GetString(file, key, "");
	return str == "" ? defval : stof(str);
};
double Config::GetDouble(const string& file, const string& key, double defval){
	if(initCounter < 1) Init();
	string str = GetString(file, key, "");
	return str == "" ? defval : stod(str);
};


}