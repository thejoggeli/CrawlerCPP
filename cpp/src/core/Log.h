#pragma once

#include <sstream>

namespace Crawler {

enum LogLevel {
	LOG_ERROR = 0, 
	LOG_WARNING = 1, 
	LOG_DEBUG = 2, 
	LOG_INFO = 3
};

class LogLevels {
private:
	static int initCounter;
public:
	static bool error;
	static bool warning;
	static bool debug;
	static bool info;
	static bool Init();
};

class LogConcator {
private:
public:
	std::stringstream stringStream;
	template<typename T>
	LogConcator& operator << (const T& val){
		stringStream << val;
		return *this;
	}
};

void Log(LogLevel level, const std::string& source, LogConcator& log);
void Log(LogLevel level, const std::string& source, const std::string& str);

void LogInfo(const std::string& source, LogConcator& log);
void LogInfo(const std::string& source, const std::string& str);

void LogDebug(const std::string& source, LogConcator& log);
void LogDebug(const std::string& source, const std::string& str);

void LogError(const std::string& source, LogConcator& log);
void LogError(const std::string& source, const std::string& str);

void LogWarning(const std::string& source, LogConcator& log);
void LogWarning(const std::string& source, const std::string& str);

}

#define iLog Crawler::LogConcator()