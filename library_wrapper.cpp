#include "library_wrapper.h"
#include "mosquitto.h"

namespace Mosquittopp {

LibraryWrapper::LibraryWrapper()
{
    mosquitto_lib_init();
}

LibraryWrapper::~LibraryWrapper()
{
    mosquitto_lib_cleanup();
}

}
