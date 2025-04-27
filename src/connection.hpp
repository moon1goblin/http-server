#pragma once

#include <boost/asio.hpp>

#include "utils/logger.hpp"
#include "utils/thread_safe_queue.hpp"

namespace http_server {
using namespace boost::asio;

class Connection {
private:
	// helper function for logs
	auto get_ip() {
		try {
			auto ip = socket_.remote_endpoint().address();
			return ip;
		} catch (boost::system::error_code ec) {
			if (ec == error::eof) {
				LOG(INFO) << "connection closed";
			} else {
				LOG(ERROR) << "failed to get ip at socket:\n" << ec.what();
			}
			throw;
		}
	}

public:
	Connection(ip::tcp::socket&& socket, Utils::ThreadSafeQueue<std::string>& incoming_queue)
		: socket_(std::move(socket))
		, incoming_queue_(incoming_queue)
	{
		LOG(INFO) << "new connection at " << get_ip();
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

		// sync because im fucking with multithreaded server model atm
		boost::system::error_code ec;
		boost::asio::read_until(socket_, read_buf_, header_delimiter, ec);

		if (ec == error::eof) {
			LOG(INFO) << "connection closed";
		}
		else if (ec) {
			LOG(ERROR) << "failed to read data from socket:\n" << ec.what();
		}
		else {
			std::string s((std::istreambuf_iterator<char>(&read_buf_)), std::istreambuf_iterator<char>());
			incoming_queue_.push(s);

			read_data();
		}

		// async_read_until(socket_, read_buf_, header_delimiter
		// 		, [&](boost::system::error_code ec, std::size_t bytes_read) {
		// 	if (ec == error::eof) {
		// 		LOG(INFO) << "connection closed";
		// 	}
		// 	else if (ec) {
		// 		LOG(ERROR) << "failed to read data from socket:\n" << ec.what();
		// 	}
		// 	else {
		// 		header_data_handler();
		// 		read_data();
		// 	}
		//       });

		// TODO: read the body too
    }

private:
    ip::tcp::socket socket_;
    streambuf read_buf_;
	Utils::ThreadSafeQueue<std::string>& incoming_queue_;
};
}
