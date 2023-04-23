#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
// #include <iostream>

namespace Crawler {

class SmartBuffer {
private:

	std::vector<uint8_t> bytes;
    std::unordered_map<std::string, unsigned int> map_str;
    std::unordered_map<unsigned int, unsigned int> map_int;

public:

	bool swapEndian = false;

    SmartBuffer(unsigned int reserveBytes=64);
    SmartBuffer(const uint8_t* bytes, unsigned int numBytes);
	~SmartBuffer();

    void SetBytes(const uint8_t* bytes, unsigned int numBytes);
	const uint8_t* GetBytes();
	unsigned int GetSize();
	void PrintPretty(bool hex = true, int lineSize = 16);

	// add string
	void AddString(unsigned int key, const char* value);
	void AddString(const std::string& key, const char* value);
	void AddString(const char* value);
	
	// add variable value
	template<typename ValType> void Add(unsigned int key, ValType value);
	template<typename ValType> void Add(const std::string& key, ValType value);
	template<typename ValType> void Add(ValType value);

	// get string
	const char* GetString(unsigned int key);
	const char* GetString(const std::string& key);

	// get variable value
	template<typename ValType> ValType Get(unsigned int key, unsigned int offset=0);
	template<typename ValType> ValType Get(const std::string& key, unsigned int offset=0);



};

}


