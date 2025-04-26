#pragma once

#include "boost/asio/streambuf.hpp"
#include <boost/asio.hpp>
#include <sys/socket.h>
#include <iostream>

namespace {
using namespace boost::asio;

class Connection : public std::enable_shared_from_this<Connection> {
public:
    Connection(ip::tcp::socket&& socket)
        : socket_(std::move(socket)) 
    {}

    // reads client's messages and queues them
    void read_data() {
        std::cout << "[read_data] im reading something" << std::endl;
        const std::string delimiter = "\r\n\r\n";
        async_read_until(socket_, read_buf_, delimiter
            , [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_read) {
                if (ec == error::eof) {
                    std::cout << "connection closed" << "std::endl";
                }
                else if (ec) {
                    std::cerr << "[read_data] error: " << std::flush;
                    std::cerr << ec.what() << std::endl;
                }
                else {
                    // TODO: im not sure it even has a move method
                         //
                    // httpMessage message;
                    // message.parse(self->read_buf_);
                    // self->read_body(message);
                    // using namespace std::chrono_literals;
                    // std::this_thread::sleep_for(2000ms);
                    std::cout << &self->read_buf_;
                    self->read_data();
                }
        });
    }

private:
    ip::tcp::socket socket_;
    streambuf read_buf_;
    // TODO: how do we know which socket to respond to?
};
}
