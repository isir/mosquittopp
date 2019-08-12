#ifndef SUBSCRIPTION_FACTORY_H
#define SUBSCRIPTION_FACTORY_H

#include "subscription.h"
#include <vector>
#include <memory>

namespace Mosquittopp {

class SubscriptionFactory
{
public:
    static std::shared_ptr<Subscription> get(mosquitto* token, std::string pattern, int qos);
    static void handle_message(mosquitto *token, Message msg);

private:
    SubscriptionFactory();

    static std::vector<std::shared_ptr<Subscription>> _subscriptions;
    static std::mutex _mutex;
};

}

#endif // SUBSCRIPTION_FACTORY_H
