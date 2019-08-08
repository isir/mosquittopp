#include "connect_helper.h"

namespace Mosquittopp {

void message_callback(struct mosquitto*, void* obj, const mosquitto_message* msg)
{
    ConnectHelper* c = reinterpret_cast<ConnectHelper*>(obj);
    c->handle_message_received(msg);
}

void connect_callback(struct mosquitto*, void* obj, int rc)
{
    ConnectHelper* c = reinterpret_cast<ConnectHelper*>(obj);
    switch (rc) {
    case 0:
        c->_status = ConnectHelper::CONNECTED;
        c->start_thread();
        break;
    case 1:
        c->_status = ConnectHelper::UNACCEPTABLE_PROTOCOL_VERSION;
        break;
    case 2:
        c->_status = ConnectHelper::IDENTIFIER_REJECTED;
        break;
    case 3:
        c->_status = ConnectHelper::BROKER_UNAVAILABLE;
        break;
    default:
        c->_status = ConnectHelper::UNKNOWN;
        break;
    }
}

void disconnect_callback(struct mosquitto*, void* obj, int rc)
{
    ConnectHelper* c = reinterpret_cast<ConnectHelper*>(obj);
    switch (rc) {
    case 0:
        c->_status = ConnectHelper::CLEAN_DISCONNECT;
        break;
    default:
        c->_status = ConnectHelper::UNEXPECTED_DISCONNECT;
        break;
    }
    c->stop_thread();
}

ConnectHelper::ConnectHelper()
    : _token(nullptr)
    , _status(UNKNOWN)
{
    _token = mosquitto_new(nullptr, true, reinterpret_cast<void*>(this));
    if (_token == nullptr) {
        throw std::runtime_error("Failed to allocate resources: " + std::to_string(errno));
    }
    mosquitto_message_callback_set(_token, message_callback);
    mosquitto_connect_callback_set(_token, connect_callback);
    mosquitto_disconnect_callback_set(_token, connect_callback);
}

ConnectHelper::~ConnectHelper()
{
    mosquitto_disconnect(_token);
    mosquitto_loop_stop(_token, false);
    mosquitto_destroy(_token);

    stop_thread();
}

bool ConnectHelper::connect(std::string hostname, int port)
{
    disconnect();

    bool res = mosquitto_connect(_token, hostname.c_str(), port, 1) == MOSQ_ERR_SUCCESS;
    if (res) {
        mosquitto_loop_start(_token);
    }
    return res;
}

void ConnectHelper::disconnect()
{
    if (_status == CONNECTED) {
        mosquitto_disconnect(_token);
        mosquitto_loop_stop(_token, false);
    }
}

ConnectHelper::Status ConnectHelper::status()
{
    return _status;
}

void ConnectHelper::start_thread()
{
    if (_thread.joinable()) ///
        return;

    _thread_condition = true;

    _thread = std::thread([this] {
        pthread_setname_np(pthread_self(), "mosquitto_helper");
        while (_thread_condition) {
            std::unique_lock<std::mutex> lock(_queue_mutex);
            _cv.wait(lock, [this] { return (_queue.size() > 0) || !_thread_condition; });

            if (_queue.empty()) {
                continue;
            }

            Message msg = _queue.front();
            _queue.pop();
            lock.unlock();

            std::lock_guard<std::mutex> lg(_sub_mutex);
            bool result;
            for (auto s : _subscriptions) {
                mosquitto_topic_matches_sub(s->pattern().data(), msg.topic().c_str(), &result);
                if (result) {
                    s->handle_message_received(msg);
                }
            }
        }
    });
}

void ConnectHelper::stop_thread()
{
    if (_thread.joinable()) {
        _thread_condition = false;
        _cv.notify_one();
        _thread.join();
    }
}

void ConnectHelper::handle_message_received(Message msg)
{
    std::lock_guard<std::mutex> lock(_queue_mutex);
    _queue.push(msg);
    _cv.notify_one();
}
}
