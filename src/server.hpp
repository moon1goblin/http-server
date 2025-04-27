#pragma once

#include <boost/asio.hpp>
#include <cstdint>
#include <optional>

#include "connection.hpp"
#include "utils/logger.hpp"
#include "utils/jthread.hpp"
#include "utils/thread_safe_queue.hpp"

namespace http_server {
using namespace boost::asio;

class Server {
public:
    Server(io_context& io_context, std::uint16_t port)
        : io_context_(io_context)
        , acceptor_(io_context_, ip::tcp::endpoint(ip::tcp::v4(), port)) {
	}

	void start() {
		accept_connections();
		respond();
		LOG(INFO) << "server started, accepting connections";
		io_context_.run();
	}

	void stop() {
		io_context_.stop();
		respond_thread_.reset();
		LOG(DEBUG) << "stopped responding to messages";

		LOG(INFO) << "server stopped";
	}

	void respond() {
		LOG(DEBUG) << "started responding to messages";
		accept_connections();
		respond_thread_.emplace(std::thread([&]() {
			while (true) {
				std::string message = std::move(incoming_queue_.pop());
				LOG(OFF) << message;
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
					Connection new_connection(std::move(*new_socket_), incoming_queue_);
					new_connection.read_data();
				}));
			}
			accept_connections();
		});
    }

private:
    io_context& io_context_;
    ip::tcp::acceptor acceptor_;
    std::optional<ip::tcp::socket> new_socket_;
	Utils::ThreadSafeQueue<std::string> incoming_queue_;
	std::optional<Utils::JThread> respond_thread_;
};
}
