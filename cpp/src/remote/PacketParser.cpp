#include "PacketParser.h"
#include "PacketType.h"
#include <cstdint>
#include "events/Event2.h"
#include <memory>

namespace Crawler {

void PacketParser::Pack(){

}

void PacketParser::Unpack(const uint8_t* bytes, unsigned int numBytes){

    std::shared_ptr<Event2> event = std::make_shared<Event2>(bytes, numBytes);

    // TODO
    // build event index here
    // event.Register<float>("f1");
    // event.Register<int>("i1");
    // event.RegisterString("str1");
    

}

}