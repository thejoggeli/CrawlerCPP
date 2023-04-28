#include "Log.h"
#include "core/Config.h"

#include <iostream>
#include <time.h>
#include <mutex>

using namespace std;

namespace Crawler {

std::mutex mtx;

static const char* COLOR_BLACK      = "\e[1;30m";
static const char* COLOR_RED        = "\e[1;31m";
static const char* COLOR_GREEN      = "\e[1;32m";
static const char* COLOR_YELLOW     = "\e[1;33m";
static const char* COLOR_BLUE       = "\e[1;34m";
static const char* COLOR_MAGENTA    = "\e[1;35m";
static const char* COLOR_CYAN       = "\e[1;36m";
static const char* COLOR_WHITE      = "\e[1;37m";
static const char* COLOR_SUFFIX     = "\e[0m";

static char timeBuffer[32];
static char prefixBuffer[64];

static const int max_str_len = 150;
static const int max_str_len_cut = max_str_len-3;

int LogLevels::initCounter = 0;
bool LogLevels::error = true;
bool LogLevels::warning = true;
bool LogLevels::debug = true;
bool LogLevels::info = true;


bool LogLevels::Init(){
    // mtx.lock();
    if(++initCounter > 1) return false;
    info = Config::GetBool("log_info", true);
    debug = Config::GetBool("log_debug", true);
    warning = Config::GetBool("log_warning", true);
    error = Config::GetBool("log_error", true);
    // mtx.unlock();
    return true;
}

static void LogInner(LogLevel level, const std::string& source, const std::string& str){

    if(level == LogLevel::LOG_INFO && !LogLevels::info) return;
    else if(level == LogLevel::LOG_DEBUG && !LogLevels::debug) return;
    else if(level == LogLevel::LOG_WARNING && !LogLevels::warning) return;
    else if(level == LogLevel::LOG_ERROR && !LogLevels::error) return;

    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime (&rawtime);
    strftime(timeBuffer,sizeof(timeBuffer),"%H:%M:%S",timeinfo);

    char levelChar = '?';
    const char* colorPrefix = "";
    const char* colorSuffix = "";
    switch(level){
        case LOG_INFO: 
            levelChar = 'I'; 
            colorPrefix = COLOR_BLUE;
            colorSuffix = COLOR_SUFFIX;
            break;
        case LOG_DEBUG: 
            levelChar = 'D';
            colorPrefix = COLOR_MAGENTA; 
            colorSuffix = COLOR_SUFFIX;
            break;
        case LOG_WARNING: 
            levelChar = 'W'; 
            colorPrefix = COLOR_YELLOW;
            colorSuffix = COLOR_SUFFIX;
            break; 
        case LOG_ERROR: 
            levelChar = 'E'; 
            colorPrefix = COLOR_RED;
            colorSuffix = COLOR_SUFFIX;
            break; 
    }

    sprintf(prefixBuffer, "%s[%c/%s/%s]%s ", colorPrefix, levelChar, timeBuffer, source.c_str(), colorSuffix);
    if(str.length() > max_str_len){
        cout << prefixBuffer << str.substr(0, max_str_len_cut) << "..." << endl;
    } else {
        cout << prefixBuffer << str << endl;
    }
}

void Log(LogLevel level, const std::string& source, LogConcator& log){
    std::lock_guard<std::mutex> lock(mtx);
    LogInner(level, source, log.stringStream.str());
}
void Log(LogLevel level, const std::string& source, const std::string& str){
    std::lock_guard<std::mutex> lock(mtx);
    LogInner(level, source, str);
}

void LogInfo(const std::string& source, LogConcator& log){ 
    std::lock_guard<std::mutex> lock(mtx);
    LogInner(LogLevel::LOG_INFO, source, log.stringStream.str()); 
};
void LogInfo(const std::string& source, const std::string& str){ 
    std::lock_guard<std::mutex> lock(mtx);
    LogInner(LogLevel::LOG_INFO, source, str);     
}

void LogDebug(const std::string& source, LogConcator& log){ 
    std::lock_guard<std::mutex> lock(mtx);
    LogInner(LogLevel::LOG_DEBUG, source, log.stringStream.str()); 
};
void LogDebug(const std::string& source, const std::string& str){ 
    std::lock_guard<std::mutex> lock(mtx);
    LogInner(LogLevel::LOG_DEBUG, source, str); 
}

void LogWarning(const std::string& source, LogConcator& log){ 
    std::lock_guard<std::mutex> lock(mtx);
    LogInner(LogLevel::LOG_WARNING, source, log.stringStream.str()); 
};
void LogWarning(const std::string& source, const std::string& str){ 
    std::lock_guard<std::mutex> lock(mtx);
    LogInner(LogLevel::LOG_WARNING, source, str); 
}

void LogError(const std::string& source, LogConcator& log){ 
    std::lock_guard<std::mutex> lock(mtx);
    LogInner(LogLevel::LOG_ERROR, source, log.stringStream.str()); 
};
void LogError(const std::string& source, const std::string& str){ 
    std::lock_guard<std::mutex> lock(mtx);
    LogInner(LogLevel::LOG_ERROR, source, str); 
}

}