#include "http_server.hpp"

int main() {
    try {
		boost::asio::io_context io_context;
		http_server::Server my_server(io_context, 6969);
        my_server.start();

    } catch (std::exception &e) {
        LOG(ERROR) << e.what();
    }

    return 0;
}
