#ifndef MOSQUITTO_SUBSCRIPTION_H
#define MOSQUITTO_SUBSCRIPTION_H

#include "message.h"
#include <functional>
#include <mutex>
#include <string>
#include <vector>

namespace Mosquittopp {

class Subscription {
    friend class SubscriptionFactory;

public:
    Subscription(mosquitto* token, std::string pattern, int qos);
    ~Subscription();

    const mosquitto* token();
    std::string pattern();
    int qos();

    template <typename T>
    inline void add_callback(T* object, void (T::*callback)(Message))
    {
        add_callback(object, std::bind(callback, object, std::placeholders::_1));
    }

    void add_callback(void* object, std::function<void(Message)> callback);
    void remove_callbacks(void* object);

private:
    void handle_message_received(Message msg);

    mosquitto* _token;
    std::string _pattern;
    int _qos;

    std::vector<std::pair<void*, std::function<void(Message)>>> _callbacks;
    std::mutex _mtx;
};

}

#endif // MOSQUITTO_SUBSCRIPTION_H
