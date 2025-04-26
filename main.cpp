#include "http_server.hpp"

int main() {
    try {
        boost::asio::io_context io_context;

        Server my_server(io_context, 6969);
        my_server.accept_connections();

        io_context.run();

    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
