#pragma once

#include <boost/asio.hpp>
#include <optional>
#include <thread>

#include "api.hpp"
#include "connection.hpp"
#include "http-parser.hpp"
#include "utils/logger.hpp"
#include "utils/jthread.hpp"

namespace web_server {
using namespace boost::asio;

class Server {
public:
    Server(std::uint16_t port)
		: io_context_()
		, acceptor_(io_context_, ip::tcp::endpoint(ip::tcp::v4(), port))
	{
	}

	void start() {
		is_running_ = true;
		accept_connections();
		handle_connections();
		LOG(INFO) << "server started, accepting connections";
		io_context_.run();
	}

	// TODO: this does not fucking work
	void stop() {
		LOG(INFO) << "attempting to stop the server";
		is_running_ = false;
		respond_thread_.join();
		io_context_.stop();
		LOG(INFO) << "server stopped";
	}

	void handle_connections() {
		// TODO: make this lambda recursive
		respond_thread_ = std::thread([&]() {
			while (true) {
				const web_server::connection_manager::Incoming_Message_type 
				Request_Message = std::move(incoming_queue_.pop());
				// checking before formulating/sending response to not waste resources
				if (Request_Message.connection_ptr == nullptr) {
					LOG(DEBUG) << "responding to a connection that was disonected";
					continue;
				}
				// make response
				std::string response = api.make_http_response(Request_Message.http_request).make_string();

				// send response
				if (Request_Message.connection_ptr == nullptr) {
					LOG(DEBUG) << "responding to a connection that was disonected";
					continue;
				}
				Request_Message.connection_ptr->write_data(std::move(response));
			}
		});
	}

    void accept_connections() {
		new_socket_.emplace(io_context_);
		acceptor_.async_accept(*new_socket_, [&](boost::system::error_code ec) {
			if (ec) {
				LOG(ERROR) << "failed to create a new connection: " << ec.what();
			}
			else {
				// Utils::JThread j_thr(std::thread([&](){
				std::make_shared<Connection>(
					std::move(*new_socket_), incoming_queue_
				)->read_data();
				// }));
			}
			accept_connections();
		});
    }

public:
	api api;
private:
	bool is_running_;
    io_context io_context_;
    ip::tcp::acceptor acceptor_;
    std::optional<ip::tcp::socket> new_socket_;
	std::thread respond_thread_;
	web_server::connection_manager::ThreadSafeQueue<web_server::connection_manager::Incoming_Message_type> incoming_queue_;
};

}
