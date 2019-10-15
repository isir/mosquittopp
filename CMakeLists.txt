project(mosquittopp CXX)

set(SOURCES
    # cmake-format: sortable
    client.cpp
    connect_factory.cpp
    connect_helper.cpp
    message.cpp
    subscription.cpp
    subscription_factory.cpp)

add_library(mosquittopp ${SOURCES})

target_link_libraries(mosquittopp mosquitto)
