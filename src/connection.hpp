#pragma once

#include "boost/asio/streambuf.hpp"
#include <boost/asio.hpp>
#include <sys/socket.h>
#include <iostream>

#include "logger.hpp"

namespace http_server {
using namespace boost::asio;

class Connection : public std::enable_shared_from_this<Connection> {
public:
    Connection(ip::tcp::socket&& socket)
        : socket_(std::move(socket)) 
    {}

    // reads client's messages and queues them
    void read_data() {
		LOG(DEBUG) << "started to read data from socket at ip: "
			<< socket_.remote_endpoint().address();

        const std::string delimiter = "\r\n\r\n";

        async_read_until(socket_, read_buf_, delimiter
            , [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_read) {
                if (ec == error::eof) {
                    LOG(INFO) << "connection closed at ip: "
						<< self->socket_.remote_endpoint().address();
                }
                else if (ec) {
                    LOG(ERROR) << "failed to read data from socket at ip: "
						<< self->socket_.remote_endpoint().address()
						<< " " << ec.what();
                }
                else {
					// TODO: make queue for incoming messages or something
                    std::cout << &self->read_buf_;
                    self->read_data();
                }
        });
    }

private:
    ip::tcp::socket socket_;
    streambuf read_buf_;
};
}
