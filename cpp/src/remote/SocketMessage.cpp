#include "SocketMessage.h"
#include <iostream>
#include <regex>

namespace Crawler {

SocketMessage::SocketMessage(std::string message) {
	AddParam("type", message);
}
std::string& SocketMessage::GetJsonString(){
	json = "{";
	std::string delim_1 = "";
	std::regex reg("^(?=.)([+-]?([0-9]*)(\\.([0-9]+))?)$");
	for(auto const& key: params){
		json += delim_1 + "\"" + key.first + "\":";
		if(key.second.size() == 1){
			const std::string& val = key.second[0];
			bool isZeroPrefixed = val[0] == '0' && val.length() > 1;
			if(!isZeroPrefixed && std::regex_match(val, reg)){
				// string needs no quotes (numbers, true, false)
				json += val;
			} else {
				// string needs quotes
				json += "\""+val+"\"";
			}
		} else {
			json += "[";
			std::string delim_2 = "";
			for(auto const& val: key.second){
				bool isZeroPrefixed = val[0] == '0' && val.length() > 1;
				if(!isZeroPrefixed && std::regex_match(val, reg)){
					// string needs no quotes (numbers, true, false)
					json += delim_2 + val;
				} else {
					// string needs quotes
					json += delim_2 + "\""+val+"\"";
				}
				delim_2 = ",";
			}
			json += "]";
		}
		delim_1 = ",";
	}
	json += "}";
	return json;
}

}
