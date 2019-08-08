#ifndef MOSQUITTO_SUBSCRIPTION_H
#define MOSQUITTO_SUBSCRIPTION_H

#include "message.h"
#include <functional>
#include <map>
#include <mutex>
#include <string>

namespace Mosquittopp {

class Subscription {
    friend class ConnectHelper;

public:
    Subscription(std::string pattern);

    inline std::string pattern()
    {
        return _pattern;
    }

    template <typename T>
    inline bool add_callback(T* object, void (T::*callback)(Message))
    {
        return add_callback(object, std::bind(callback, object, std::placeholders::_1));
    }

    inline bool add_callback(void* object, std::function<void(Message)> callback)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        return _callbacks.insert({ object, callback }).second;
    }

    void remove_callbacks(void* object);

private:
    void handle_message_received(Message msg);

    std::string _pattern;
    std::map<void*, std::function<void(Message)>> _callbacks;
    std::mutex _mtx;
};

}

#endif // MOSQUITTO_SUBSCRIPTION_H
