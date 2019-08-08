#include "message.h"

namespace Mosquittopp {

Message::Message()
{
}

Message::Message(const mosquitto_message* msg)
    : _topic(msg->topic)
    , _payload(reinterpret_cast<char*>(msg->payload), static_cast<std::size_t>(msg->payloadlen))
{
}

std::string Message::topic()
{
    return _topic;
}

std::string Message::payload()
{
    return _payload;
}

}
