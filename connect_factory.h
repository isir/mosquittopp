#ifndef CONNECT_FACTORY_H
#define CONNECT_FACTORY_H

#include "connect_helper.h"
#include <map>
#include <mutex>
#include <string>

namespace Mosquittopp {

class ConnectFactory {
public:
    static ConnectFactory& instance();

    ~ConnectFactory();

    std::shared_ptr<ConnectHelper> from_identifier(std::string identifier);

private:
    ConnectFactory();

    std::map<std::string, std::shared_ptr<ConnectHelper>> _helpers;
    std::mutex _helpers_mutex;
};

}

#endif // CONNECT_FACTORY_H
