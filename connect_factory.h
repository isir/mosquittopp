#ifndef CONNECT_FACTORY_H
#define CONNECT_FACTORY_H

#include "connect_helper.h"
#include <map>
#include <mutex>
#include <string>

namespace Mosquittopp {

class ConnectFactory {
public:
    static std::shared_ptr<ConnectHelper> from_identifier(std::string identifier);

private:
    ConnectFactory();

    static std::map<std::string, std::shared_ptr<ConnectHelper>> _helpers;
    static std::mutex _helpers_mutex;
};

}

#endif // CONNECT_FACTORY_H
