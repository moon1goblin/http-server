#pragma once

#include "boost/asio/io_context.hpp"
#include <boost/asio.hpp>
#include <cstdint>
#include <optional>
#include <sys/socket.h>

#include "connection.hpp"
#include "logger.hpp"

namespace http_server {
using namespace boost::asio;

class Server {
public:
    Server(io_context& io_context, std::uint16_t port)
        : io_context_(io_context)
        , acceptor_(io_context_, ip::tcp::endpoint(ip::tcp::v4(), port)) {
	}

	// Server(std::uint16_t port) {
	// 	// io_context& io_context_ = *(new io_context());
	// 	io_context io_context_;
	// 	ip::tcp::acceptor acceptor_(io_context_, ip::tcp::endpoint(ip::tcp::v4(), port));
	// 	// ip::tcp::acceptor acceptor_(io_context_, ip::tcp::endpoint(ip::tcp::v4(), port));
	// 	// ip::tcp::acceptor acceptor_ = *(new ip::tcp::acceptor(io_context_, ip::tcp::endpoint(ip::tcp::v4(), port)));
	// }

	void start() {
		LOG(INFO) << "server started, accepting connections";
		accept_connections();
		io_context_.run();
	}

	// // TODO: make a stop method
	// void stop() {
	// }

    void accept_connections() {
		new_socket_.emplace(io_context_);
		acceptor_.async_accept(*new_socket_, [&](boost::system::error_code ec) {
			if (ec) {
				LOG(ERROR) << "failed to create a new connection: " << ec.what();
			}
			else {
				LOG(INFO) << "new connection at ip: " << new_socket_->remote_endpoint().address();

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
