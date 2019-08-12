#include "client.h"
#include "connect_factory.h"
#include "subscription_factory.h"

namespace Mosquittopp {

LibraryWrapper Client::_lib_wrapper;

Client::Client()
{
}

Client::~Client()
{
}

bool Client::connect(std::string hostname, int port)
{
    _con_helper = ConnectFactory::from_identifier(hostname + ":" + std::to_string(port));
    return _con_helper->connect(hostname, port);
}

void Client::disconnect()
{
    if (connected()) {
        _con_helper->disconnect();
    }
}

bool Client::connected()
{
    if (!_con_helper) {
        return false;
    }
    return _con_helper->status() == ConnectHelper::CONNECTED;
}

LibraryVersion Client::lib_version()
{
    int major, minor, revision;
    mosquitto_lib_version(&major, &minor, &revision);
    return LibraryVersion { major, minor, revision };
}

void Client::set_will(std::string topic, std::string payload, QOS qos, bool retain)
{
    if (!connected()) {
        return;
    }
    mosquitto_will_set(_con_helper->_token, topic.data(), static_cast<int>(payload.length()), reinterpret_cast<void*>(const_cast<char*>(payload.data())), qos, retain);
}

void Client::clear_will()
{
    if (!connected()) {
        return;
    }
    mosquitto_will_clear(_con_helper->_token);
}

void Client::set_login_info(std::string username, std::string password)
{
    if (!connected()) {
        return;
    }
    mosquitto_username_pw_set(_con_helper->_token, username.data(), password.data());
}

std::shared_ptr<Subscription> Client::subscribe(std::string pattern, QOS qos)
{
    return SubscriptionFactory::get(_con_helper->_token, pattern, qos);
}

void Client::publish(std::string topic, const void* payload, std::size_t payload_len, QOS qos, bool retain)
{
    if (!connected()) {
        return;
    }
    mosquitto_publish(_con_helper->_token, nullptr, topic.c_str(), static_cast<int>(payload_len), payload, qos, retain);
}
}
