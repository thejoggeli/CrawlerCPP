#include "Strings.h"
#include <algorithm>
#include <limits.h>
#include <sstream>
#include <iomanip>
#include <unistd.h>

using namespace std;

namespace Crawler {
namespace Strings {

vector<string> Split(const string &str, const string &delimiter){
	vector<string> split;
	size_t len = delimiter.length();
	size_t pos = 0;
	string s = str;
	string token;
	while((pos = s.find(delimiter)) != string::npos){
		token = s.substr(0, pos);
		split.push_back(token);
		s.erase(0, pos + len);
	}
	split.push_back(s);
	return split;
}

string Trim(const string &str){
	auto  wsfront = find_if_not(str.begin(),str.end(),[](int c){return isspace(c);});
	return string(wsfront,find_if_not(str.rbegin(),string::const_reverse_iterator(wsfront),[](int c){return isspace(c);}).base());
}

string RemoveMultipleWhitespaces(const string &in){
    size_t pos;
    string out = in;
    while((pos = out.find("  ")) != string::npos){
        out.replace(pos, 1, "");
    }
    return out;
}

string ReplaceAll(const std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    string out = str;
    while((start_pos = out.find(from, start_pos)) != std::string::npos) {
        out.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return out;
}

bool StartsWith(const string &str, const string &with){
	return str.rfind(with, 0) == 0;
}

string GetAppName(){
	char result[1024];
	ssize_t count = readlink( "/proc/self/exe", result, 1024);
	string exec = std::string(result, (count > 0) ? count : 0);
	int pos = exec.find_last_of("\\/");
	return exec.substr(pos+1);
}

string GetExecDir(){
	char result[1024];
	ssize_t count = readlink( "/proc/self/exe", result, 1024);
	string exec = std::string(result, (count > 0) ? count : 0);
	return exec.substr(0, exec.find_last_of("\\/"));
}

string GetConfigPath(const std::string& filename){
	string dir = CRAWLER_CONFIG_PATH;
	return dir + "/" + filename;
}

string RoundToFixed(float number, int decimals){
	std::stringstream ss;
	ss << std::fixed << std::setprecision(decimals) << number;
	return ss.str();
}

}
}