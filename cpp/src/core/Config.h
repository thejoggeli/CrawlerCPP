#pragma once

#include <string>
#include <map>
#include <vector>

namespace Crawler {

class Config {
private:
	static int initCounter;
	Config();
public:
	enum class Target {User, Default, Runtime};
	static bool Init();
	static void Set(const std::string& key, const std::string& value, Config::Target target);
	static void Set(const std::string& key, int value, Config::Target target);
	static void Set(const std::string& key, float value, Config::Target target);
	static void Set(const std::string& key, double value, Config::Target target);
	static void ReadFile(const std::string& filename, std::map<std::string, std::string>& target);
	static int GetInt(const std::string& key, int defval = 0);
	static std::string GetString(const std::string& key, const std::string& defval = "");
	static bool GetBool(const std::string& key, bool defval = false);
	static float GetFloat(const std::string& key, float defval = 0);
	static double GetDouble(const std::string& key, double defval = 0);
	static void WriteFile(Config::Target target);
	static void SetLauncherArgs(std::vector<std::string>& args);
	static std::string TargetToPath(Config::Target target);
	static std::map<std::string, std::string>& TargetToMap(Config::Target target);
};

}