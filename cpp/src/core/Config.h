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

	static bool Init();

	static void ReadFile			(const std::string& file);
	static void WriteFile			(const std::string& file);

	static void Set					(const std::string& file, const std::string& key, const std::string& value);
	static void Set					(const std::string& file, const std::string& key, int value);
	static void Set					(const std::string& file, const std::string& key, float value);
	static void Set					(const std::string& file, const std::string& key, double value);

	static int GetInt				(const std::string& file, const std::string& key, int defval = 0);
	static std::string GetString	(const std::string& file, const std::string& key, const std::string& defval = "");
	static bool GetBool				(const std::string& file, const std::string& key, bool defval = false);
	static float GetFloat			(const std::string& file, const std::string& key, float defval = 0);
	static double GetDouble			(const std::string& file, const std::string& key, double defval = 0);

};

}