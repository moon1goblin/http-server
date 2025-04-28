#pragma once

#include <boost/asio.hpp>
#include <memory>

#include "http-parser.hpp"
#include "utils/logger.hpp"
#include "utils/thread_safe_queue.hpp"

namespace web_server {
using namespace boost::asio;

// forward declaration
class Connection;

struct Incoming_Message_type {
	web_server::HTTP::Request http_request;
	std::shared_ptr<Connection> connection_ptr;

	Incoming_Message_type(web_server::HTTP::Request&& parsed_msg, std::shared_ptr<Connection> connection_ptr) 
		: http_request(std::move(parsed_msg))
		, connection_ptr(connection_ptr) {
	}
};

class Connection: public std::enable_shared_from_this<Connection> {
public:
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

	Connection(ip::tcp::socket&& socket, Utils::ThreadSafeQueue<web_server::Incoming_Message_type>& incoming_queue)
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
	void write_data(const std::string& message_str) {
		boost::system::error_code ec;
		boost::asio::write(socket_, boost::asio::buffer(message_str), ec);

		if (ec == error::eof) {
			LOG(INFO) << "connection closed";
		}
		else if (ec) {
			LOG(ERROR) << "failed to send data to socket:\n" << ec.what();
		}
		else {
			LOG(DEBUG) << "sent message:\n" << message_str
				<< "\nto socket at ip: " << get_ip();
		}
	}

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
			incoming_queue_.push(Incoming_Message_type(
					web_server::HTTP::Request(read_buf_), shared_from_this()
			));
			// incoming_queue_.push(msg);

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
	Utils::ThreadSafeQueue<web_server::Incoming_Message_type>& incoming_queue_;
};

}
