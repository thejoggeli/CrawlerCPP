#pragma once

#define iLog (Crawler::LogConcator::instance)

#include <sstream>

namespace Crawler {

enum LogLevel {
	LOG_ERROR = 0, LOG_DEBUG = 1, LOG_INFO = 2
};

class LogLevels {
private:
	static int initCounter;
public:
	static bool error;
	static bool debug;
	static bool info;
	static bool Init();
};

class LogConcator {
private:
public:
	std::stringstream stringStream;
	static LogConcator instance;
	template<typename T>
	LogConcator& operator << (const T& val){
		stringStream << val;
		return *this;
	}
};

void Log(LogConcator& log);
void Log(const std::string& str);
void Log(LogLevel level, const std::string& source, LogConcator& log);
void Log(LogLevel level, const std::string& source, const std::string& str);

void LogInfo(LogConcator& log);
void LogInfo(const std::string& str);
void LogInfo(const std::string& source, LogConcator& log);
void LogInfo(const std::string& source, const std::string& str);

void LogDebug(LogConcator& log);
void LogDebug(const std::string& str);
void LogDebug(const std::string& source, LogConcator& log);
void LogDebug(const std::string& source, const std::string& str);

void LogError(LogConcator& log);
void LogError(const std::string& str);
void LogError(const std::string& source, LogConcator& log);
void LogError(const std::string& source, const std::string& str);


}