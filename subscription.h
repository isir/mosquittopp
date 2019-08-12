#ifndef MOSQUITTO_SUBSCRIPTION_H
#define MOSQUITTO_SUBSCRIPTION_H

#include "message.h"
#include <functional>
#include <map>
#include <mutex>
#include <string>

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
    inline bool add_callback(T* object, void (T::*callback)(Message))
    {
        return add_callback(object, std::bind(callback, object, std::placeholders::_1));
    }

    bool add_callback(void* object, std::function<void(Message)> callback);
    void remove_callbacks(void* object);

private:
    void handle_message_received(Message msg);

    mosquitto* _token;
    std::string _pattern;
    int _qos;

    std::map<void*, std::function<void(Message)>> _callbacks;
    std::mutex _mtx;
};

}

#endif // MOSQUITTO_SUBSCRIPTION_H
