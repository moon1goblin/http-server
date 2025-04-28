#pragma once

#include <boost/asio.hpp>
#include <cstddef>
#include <optional>

#include "connection.hpp"
#include "http-parser.hpp"
#include "utils/logger.hpp"
#include "utils/jthread.hpp"
#include "utils/thread_safe_queue.hpp"
#include "../api/api.hpp"

namespace web_server {
using namespace boost::asio;

class Server {
public:
    Server(io_context& io_context, std::uint16_t port)
        : io_context_(io_context)
        , acceptor_(io_context_, ip::tcp::endpoint(ip::tcp::v4(), port)) {
	}

	void start() {
		accept_connections();
		handle_connections();
		LOG(INFO) << "server started, accepting connections";
		io_context_.run();
	}

	void stop() {
		io_context_.stop();
		respond_thread_.reset();
		LOG(DEBUG) << "stopped responding to messages";

		LOG(INFO) << "server stopped";
	}

	void handle_connections() {
		LOG(DEBUG) << "started responding to messages";
		respond_thread_.emplace(std::thread([&]() {
			while (true) {
				Incoming_Message_type http_msg = std::move(incoming_queue_.pop());

				// checking before formulating/sending response to not waste resources
				if (http_msg.connection_ptr == nullptr) {
					LOG(DEBUG) << "responding to a connection that was disonected";
					continue;
				}

				std::string response = api::handle_request(http_msg).MAKE_RESPONSE();

				if (http_msg.connection_ptr == nullptr) {
					LOG(DEBUG) << "responding to a connection that was disonected";
					continue;
				}

				http_msg.connection_ptr->write_data(response);

			}
		}));
	}

    void accept_connections() {
		new_socket_.emplace(io_context_);
		acceptor_.async_accept(*new_socket_, [&](boost::system::error_code ec) {
			if (ec) {
				LOG(ERROR) << "failed to create a new connection: " << ec.what();
			}
			else {
				Utils::JThread j_thr(std::thread([&](){
					std::make_shared<Connection>(
						std::move(*new_socket_), incoming_queue_
					)->read_data();
				}));
			}
			accept_connections();
		});
    }

private:
    io_context& io_context_;
    ip::tcp::acceptor acceptor_;
    std::optional<ip::tcp::socket> new_socket_;
	std::optional<Utils::JThread> respond_thread_;
	Utils::ThreadSafeQueue<web_server::Incoming_Message_type> incoming_queue_;
};
}
