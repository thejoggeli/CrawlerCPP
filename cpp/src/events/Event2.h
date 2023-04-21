#pragma once

#include <string>
#include <vector>
#include <unordered_map>
// #include <iostream>

namespace Crawler {

enum class EventType3 {
	Base = 0x00,
	Button = 0x0010,
	Joystick = 0x0011,
};

struct Event3 {
	EventType3 type = EventType.Base;

    bool fromClient = false;
    int clientId = -1;
	
	virtual void pack(){}
	virtual void unpack(){}
};

struct ButtonEvent3 : public Event {
	EventType3 type = EventType.Button;
	
};

struct JoystickEvent3 : public Event {
	EventType3 type = EventType.Joystick;
};

class Event2 {
private:

	std::vector<uint8_t> bytes;
    std::unordered_map<std::string, unsigned int> map;

public:

    bool fromClient = false;
    int clientId = -1;

	std::string type;
    
    Event2();
    Event2(const uint8_t* bytes, unsigned int numBytes);
	~Event2();

	void AddString(const char* key, const char* value);
	const char* GetString(const char* key);
	
	template<typename T>
	void Add(const char* key, T value){
        unsigned int bytePos = this->bytes.size();
		if(!map.contains(key)){
        	map[std::string(key)] = bytePos;
		}
		uint8_t* bytes = (uint8_t*)(&value);
		for(int i = 0; i < sizeof(T); i++){
			this->bytes.push_back(bytes[i]);
            // std::cout << (int)bytes[i] << std::endl;
		}
	};

	template<typename T>
	T Get(const char* key, unsigned int offset=0){
        unsigned int bytePos = map[key];
		uint8_t* bytes = &(this->bytes.data()[bytePos+offset*sizeof(T)]);
		// for(int i = 0; i < sizeof(T); i++){
        //     std::cout << (int)bytes[i] << std::endl;
		// }
		return *((T*)bytes);
	}

    void SetBytes(const uint8_t* bytes, unsigned int numBytes);
	
};

}


