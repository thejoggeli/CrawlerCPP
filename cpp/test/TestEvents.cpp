#include "core/Log.h"
#include "events/Event2.h"

using namespace Crawler;


int main(){

    Event2 event = Event2();

    event.Add<float>("v1", 1337.3383f);
    event.AddString("str1", "hallo welt");
    event.Add<float>("v2", -1.5383f);
    event.AddString("str2", "hallo welt xyz");
    event.Add<float>("v3", 0.0f);
    event.Add<uint16_t>("v4", 512);
    event.Add<float>("x", 1);
    event.Add<float>("x", 2);
    event.Add<float>("x", 3);

    float v1 = event.Get<float>("v1");
    std::string str1 = event.GetString("str1");
    float v2 = event.Get<float>("v2");
    float v3 = event.Get<float>("v3");
    std::string str2 = event.GetString("str2");
    uint16_t v4 = event.Get<uint16_t>("v4");

    float x1 = event.Get<float>("x", 0);
    float x2 = event.Get<float>("x", 1);
    float x3 = event.Get<float>("x", 2);

    LogInfo("TestEvents", iLog << v1);
    LogInfo("TestEvents", iLog << v2);
    LogInfo("TestEvents", iLog << v3);
    LogInfo("TestEvents", iLog << v4);
    LogInfo("TestEvents", iLog << str1);
    LogInfo("TestEvents", iLog << str2);
    LogInfo("TestEvents", iLog << x1);
    LogInfo("TestEvents", iLog << x2);
    LogInfo("TestEvents", iLog << x3);

    return EXIT_SUCCESS;
}