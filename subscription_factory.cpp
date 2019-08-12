#include "subscription_factory.h"

namespace Mosquittopp {

std::vector<std::shared_ptr<Subscription>> SubscriptionFactory::_subscriptions;
std::mutex SubscriptionFactory::_mutex;

SubscriptionFactory::SubscriptionFactory()
{
}

std::shared_ptr<Subscription> SubscriptionFactory::get(mosquitto* token, std::string pattern, int qos)
{
    std::shared_ptr<Subscription> ptr;
    if (mosquitto_sub_topic_check(pattern.c_str()) == MOSQ_ERR_SUCCESS) {
        for (auto s : _subscriptions) {
            if (s->token() == token && s->pattern() == pattern) {
                return s;
            }
        }
        ptr = std::make_shared<Subscription>(token, pattern, qos);
        _subscriptions.push_back(ptr);
    }
    return ptr;
}

void SubscriptionFactory::handle_message(mosquitto* token, Message msg)
{
    std::lock_guard<std::mutex> lock(_mutex);
    bool result;
    for (auto s : _subscriptions) {
        if (s->token() == token) {
            mosquitto_topic_matches_sub(s->pattern().c_str(), msg.topic().c_str(), &result);
            if (result) {
                s->handle_message_received(msg);
            }
        }
    }
}

}
