#ifndef MOSQUITTO_MESSAGE_H
#define MOSQUITTO_MESSAGE_H

#include <mosquitto.h>
#include <string>

namespace Mosquittopp {

class Message {
public:
    Message();
    Message(const mosquitto_message* msg);

    std::string topic();
    std::string payload();

private:
    std::string _topic;
    std::string _payload;
};

}

#endif // MOSQUITTO_MESSAGE_H
