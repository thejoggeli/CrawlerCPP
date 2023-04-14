#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

namespace Crawler {

class SocketMessage {
private:
	std::unordered_map<std::string, std::vector<std::string>> params;
	std::string json;
public:
	SocketMessage(std::string message);
	template<typename T>
	void AddParam(const std::string& key, const T& val){
		std::stringstream ss;
		ss << val;
		params[key].push_back(ss.str());
	}
	std::string& GetJsonString();
};

}
