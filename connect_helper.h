#ifndef CONNECT_HELPER_H
#define CONNECT_HELPER_H

#include "message.h"
#include "subscription.h"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace Mosquittopp {

class ConnectHelper {
public:
    enum Status {
        CONNECTED,
        UNACCEPTABLE_PROTOCOL_VERSION,
        IDENTIFIER_REJECTED,
        BROKER_UNAVAILABLE,
        CLEAN_DISCONNECT,
        UNEXPECTED_DISCONNECT,
        UNKNOWN
    };

    friend class Client;
    friend void message_callback(struct mosquitto*, void*, const mosquitto_message*);
    friend void connect_callback(struct mosquitto*, void*, int);
    friend void disconnect_callback(struct mosquitto*, void*, int);

    ConnectHelper();
    ~ConnectHelper();

    bool connect(std::string hostname, int port);
    void disconnect();

    Status status();

private:
    void start_thread();
    void stop_thread();

    void handle_message_received(Message msg);

    mosquitto* _token;

    Status _status;

    std::thread _thread;
    std::atomic<bool> _thread_condition;

    std::mutex _connect_mutex;
    std::condition_variable _connect_cv;

    std::queue<Message> _queue;
    std::mutex _queue_mutex;
    std::condition_variable _queue_cv;
};

}

#endif // CONNECT_HELPER_H
