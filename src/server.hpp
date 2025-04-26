#pragma once

#include "boost/asio/io_context.hpp"
#include <boost/asio.hpp>
#include <cstdint>
#include <optional>
#include <iostream>
#include <sys/socket.h>
#include <thread>

#include "connection.hpp"

namespace  {
using namespace boost::asio;

class Server {
public:
    Server(io_context& io_context, std::uint16_t port)
        : io_context_(io_context)
        , acceptor_(io_context_, ip::tcp::endpoint(ip::tcp::v4(), port)) {}

    void accept_connections() {
        new_socket_.emplace(io_context_);
        acceptor_.async_accept(*new_socket_, [&](boost::system::error_code ec) {
            if (ec) {
                std::cerr << "[read_data] error: " << std::flush;
                std::cerr << ec.what() << std::endl;
            }
            else {
                std::cout << "[acceptor] i got a connection at " << new_socket_->remote_endpoint().address() << std::endl;

                std::make_shared<Connection>(std::move(*new_socket_))->read_data();
            }

            accept_connections();
        });
    }

private:
    io_context& io_context_;
    ip::tcp::acceptor acceptor_;
    std::optional<ip::tcp::socket> new_socket_;
};
}
