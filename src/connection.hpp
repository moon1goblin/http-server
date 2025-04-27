#pragma once

#include <boost/asio.hpp>

#include "logger.hpp"

namespace http_server {
using namespace boost::asio;

class Connection : public std::enable_shared_from_this<Connection> {
private:
	// helper function for logs
	auto get_ip() {
		return socket_.remote_endpoint().address();
	}

public:
    Connection(ip::tcp::socket&& socket)
        : socket_(std::move(socket)) {
	}

	void header_data_handler() {
		LOG(DEBUG) << "handling header data at ip: "
			<< get_ip();
		std::cout << &read_buf_;
	}

	// void body_data_handler() {
	// 	LOG(DEBUG) << "handling body data at ip: "
	// 		<< get_ip();
	// }

    // reads client's messages and queues them
    void read_data() {
		LOG(DEBUG) << "reading data from socket at ip: "
			<< get_ip();

        const std::string header_delimiter = "\r\n\r\n";
		// read_data_untill(header_delimiter, [&](){
		// 		this->handle_header_data();
		// });

		async_read_until(socket_, read_buf_, header_delimiter
		, [&, self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_read) {
			if (ec == error::eof) {
				LOG(INFO) << "connection closed at ip: "
					<< get_ip();
			}
			else if (ec) {
				LOG(ERROR) << "failed to read data from socket at ip: "
					<< get_ip() << " " << ec.what();
			}
			else {
				// TODO: cout the buffer contents without emptying it
				self->header_data_handler();
				read_data();
			}
        });
		// TODO: read the body too

		// // not how it works, just get the body size in the parsed header and read for that long
		// const std::string body_delimiter = "\r\n\r\n";
		// read_data_untill(body_delimiter, [&](){
		// 		this->handle_body_data();
		// });
    }

private:
    ip::tcp::socket socket_;
    streambuf read_buf_;
};
}
