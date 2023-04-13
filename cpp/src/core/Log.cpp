#include "Log.h"
#include "core/Config.h"

#include <iostream>
#include <time.h>

using namespace std;

namespace Crawler {

static char timeBuffer[32];
static char prefixBuffer[64];

static const int max_str_len = 150;
static const int max_str_len_cut = max_str_len-3;

LogConcator LogConcator::instance = LogConcator();

int LogLevels::initCounter = 0;
bool LogLevels::error = true;
bool LogLevels::debug = true;
bool LogLevels::info = true;


bool LogLevels::Init(){
	if(++initCounter > 1) return false;
	info = Config::GetBool("log_info", true);
	debug = Config::GetBool("log_debug", true);
	error = Config::GetBool("log_error", true);
	return true;
}

void Log(LogConcator& log){
	Log(LOG_DEBUG, "Debug", log);
}
void Log(const std::string& str){
	Log(LOG_DEBUG, "Debug", str);
}

void Log(LogLevel level, const std::string& source, LogConcator& log){
	Log(level, source, log.stringStream.str());
	log.stringStream.str(std::string());
}
void Log(LogLevel level, const std::string& source, const std::string& str){

    if(level == LogLevel::LOG_INFO && !LogLevels::info) return;
    else if(level == LogLevel::LOG_DEBUG && !LogLevels::debug) return;
    else if(level == LogLevel::LOG_ERROR && !LogLevels::error) return;

	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime (&rawtime);
	strftime(timeBuffer,sizeof(timeBuffer),"%H:%M:%S",timeinfo);

    char levelChar;
    switch(level){
		case LOG_INFO: levelChar = 'I'; break;
		case LOG_DEBUG: levelChar = 'D'; break;
		case LOG_ERROR: levelChar = 'E'; break; 
        default: levelChar = 'E'; break;
    }

	sprintf(prefixBuffer, "%c %s %s ", levelChar, timeBuffer, source.c_str());
	if(str.length() > max_str_len){
        cout << prefixBuffer << str.substr(0, max_str_len_cut) << "..." << endl;
	} else {
		cout << prefixBuffer << str << endl;
	}
}

void LogInfo(LogConcator& log){ Log(LogLevel::LOG_INFO, "Info", log ); }
void LogInfo(const std::string& str){ Log(LogLevel::LOG_INFO, "Info", str ); }
void LogInfo(const std::string& source, LogConcator& log){ Log(LogLevel::LOG_INFO, source, log ); };
void LogInfo(const std::string& source, const std::string& str){ Log(LogLevel::LOG_INFO, source, str ); }

void LogDebug(LogConcator& log){ Log(LogLevel::LOG_INFO, "Debug", log ); }
void LogDebug(const std::string& str){ Log(LogLevel::LOG_INFO, "Debug", str ); }
void LogDebug(const std::string& source, LogConcator& log){ Log(LogLevel::LOG_DEBUG, source, log ); };
void LogDebug(const std::string& source, const std::string& str){ Log(LogLevel::LOG_DEBUG, source, str ); }

void LogError(LogConcator& log){ Log(LogLevel::LOG_INFO, "Error", log ); }
void LogError(const std::string& str){ Log(LogLevel::LOG_INFO, "Error", str ); }
void LogError(const std::string& source, LogConcator& log){ Log(LogLevel::LOG_ERROR, source, log ); };
void LogError(const std::string& source, const std::string& str){ Log(LogLevel::LOG_ERROR, source, str ); }

}