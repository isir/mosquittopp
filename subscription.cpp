#include "subscription.h"

namespace Mosquittopp {

Subscription::Subscription(mosquitto* token, std::string pattern, int qos)
    : _token(token)
    , _pattern(pattern)
    , _qos(qos)
{
    int ret = mosquitto_subscribe(_token, nullptr, _pattern.c_str(), _qos);
    if (ret != MOSQ_ERR_SUCCESS) {
        throw std::runtime_error("mosquitto_subscribe failed with error " + std::to_string(ret) + ": " + mosquitto_strerror(ret));
    }
}

Subscription::~Subscription()
{
    if (_token) {
        mosquitto_unsubscribe(_token, nullptr, _pattern.c_str());
    }
}

const mosquitto* Subscription::token()
{
    return _token;
}

std::string Subscription::pattern()
{
    return _pattern;
}

int Subscription::qos()
{
    return _qos;
}

void Subscription::add_callback(void* object, std::function<void(Message)> callback)
{
    std::lock_guard<std::mutex> lock(_mtx);
    _callbacks.push_back({ object, callback });
}

void Subscription::remove_callbacks(void* object)
{
    std::lock_guard<std::mutex> lock(_mtx);
    for (auto it = _callbacks.begin(); it < _callbacks.end();) {
        if (it->first == object) {
            it = _callbacks.erase(it);
        } else {
            ++it;
        }
    }
}

void Subscription::handle_message_received(Message msg)
{
    std::lock_guard<std::mutex> lock(_mtx);
    for (auto cb : _callbacks) {
        cb.second(msg);
    }
}

}
