#pragma once

#include <boost/asio.hpp>
#include <memory>

#include "http-parser.hpp"
#include "utils/logger.hpp"
#include "connection-manager.hpp"

namespace web_server {
using namespace boost::asio;

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

	Connection(ip::tcp::socket&& socket, connection_manager::ThreadSafeQueue<web_server::connection_manager::Incoming_Message_type>& incoming_queue)
		: socket_(std::move(socket))
		, incoming_queue_(incoming_queue)
	{
		LOG(INFO) << "new connection at " << get_ip();
	}

    // reads client's messages and queues them
	void write_data(std::string&& message_str) {
		async_write(socket_, boost::asio::buffer(std::move(message_str))
			, [&](boost::system::error_code ec, std::size_t bytes_read) {
			if (ec == error::eof) {
				LOG(INFO) << "connection closed";
				return;
			}
			else if (ec) {
				LOG(ERROR) << "failed to send data to socket:\n" << ec.what();
				return;
			}
			LOG(DEBUG) << "sent message to socket at ip: " << get_ip();
		});
	}

    void read_data() {
		LOG(DEBUG) << "reading data from socket at ip: "
			<< get_ip();

        const std::string header_delimiter = "\r\n\r\n";

		async_read_until(socket_, read_buf_, header_delimiter
				, [&, self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_read) {
			if (ec == error::eof) {
				LOG(INFO) << "connection closed";
				return;
			}
			else if (ec) {
				LOG(ERROR) << "failed to read data from socket:\n" << ec.what();
				return;
			}
			LOG(DEBUG) << "read the headers";

			// i kept getting a dangling reference in the callback
			// so fuck it shared_ptr it is
			auto Request_ptr = std::make_shared<web_server::HTTP::Request>();
			Request_ptr->SetHeaders(read_buf_);

			LOG(INFO) << "connection at ip " << get_ip()
				<< " requested " << Request_ptr->method
				<< " " << Request_ptr->directory;

			// we may have already read part of the body in the read_untill
			std::size_t bytes_to_read = Request_ptr->GetContentLength()
				- std::min(Request_ptr->GetContentLength(), read_buf_.size());

			async_read(socket_, read_buf_, transfer_exactly(bytes_to_read)
					, [&, Request_ptr, self](boost::system::error_code ec, std::size_t bytes_read) {
				if (ec == error::eof) {
					LOG(INFO) << "connection closed";
					return;
				}
				else if (ec) {
					LOG(ERROR) << "failed to read data from socket:\n" << ec.what();
					return;
				}
				LOG(DEBUG) << "read the body";

				Request_ptr->SetBody(read_buf_);

				incoming_queue_.push(web_server::connection_manager::Incoming_Message_type(
						std::move(*Request_ptr), self));
				// auto shit = incoming_queue_.pop();
				// LOG(ERROR) << "fuck me blyat " << shit.http_request.body;

				// for (const auto& elem : incoming_queue_.queue_) {
				// 	std::cout << elem << " ";
				// }
				// std::cout << std::endl;
				read_data();
			});
		});

    }

private:
    ip::tcp::socket socket_;
    streambuf read_buf_;
	web_server::connection_manager::ThreadSafeQueue<web_server::connection_manager::Incoming_Message_type>& incoming_queue_;
};

}
