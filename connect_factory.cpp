#include "connect_factory.h"

namespace Mosquittopp {

ConnectFactory::ConnectFactory()
{
    mosquitto_lib_init();
}

ConnectFactory::~ConnectFactory()
{
    mosquitto_lib_cleanup();
}

ConnectFactory& ConnectFactory::instance()
{
    static ConnectFactory f;
    return f;
}

std::shared_ptr<ConnectHelper> ConnectFactory::from_identifier(std::string identifier)
{
    std::lock_guard<std::mutex> lock(_helpers_mutex);

    if (_helpers.find(identifier) != _helpers.end()) {
        return _helpers.at(identifier);
    }
    auto ret = _helpers.insert(std::make_pair(identifier, std::make_shared<ConnectHelper>()));
    if (ret.second) {
        return ret.first->second;
    }
    throw std::runtime_error("Failed to insert/get key " + identifier);
}

}
