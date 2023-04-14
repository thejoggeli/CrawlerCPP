#pragma once

#include <string>
#include <vector>
#include <memory>

namespace Crawler {

class Event;
class MessageEvent;
class EventListener;

class EventManager {
private:
	static int initCounter;
	EventManager();
public:
	static unsigned int eventCounterTemp;
	static unsigned int eventCounterTotal;
	static bool Init();
	static void AddEvent(const std::shared_ptr<Event>& event);
	static void PopNewEvents();
	static std::shared_ptr<Event> ParseMessage(const std::string& str);
	static void Update();

	static void SubscribeMessage(const std::string& message, void* obj, void (*handler)(void*, MessageEvent&));
	static void UnsubscribeMessage(const std::string& message, void* obj);
	static void UnsubscribeMessagesAll(void* obj);

};


}

