#pragma once

#include <boost/asio.hpp>
#include <cstddef>
#include <optional>

#include "connection.hpp"
#include "http-parser.hpp"
#include "utils/logger.hpp"
#include "utils/jthread.hpp"
#include "utils/thread_safe_queue.hpp"

namespace web_server {
using namespace boost::asio;

class Server {
public:
	class api_type{ 
		public:
			api_type() = default;

			using handler_type = std::function<void(const HTTP::Request&, HTTP::Response&)>;

			// has a custom < for the map to work, see the end of this file
			struct route_type {
				std::string method;
				std::string directory;

				// TODO: learn the fucking move constructors already
				route_type(const std::string& method, const std::string& directory)
					: method(method)
					, directory(directory) {
				}
			};

			HTTP::Response make_http_response(const HTTP::Request& Request) {
				auto iter_route = routes_map.find(route_type(Request.method, Request.directory));

				// default not found page
				if (iter_route == routes_map.end()) {
					HTTP::Response Response;
					Response.set_status("404 (Not Found)");
					Response.set_content("<h1>404 page not found :(</h1>", "text/html");
					return Response;
				}

				handler_type cur_handler = iter_route->second;

				HTTP::Response Response;
				cur_handler(Request, Response);
				return Response;
			}

			void add_route(const std::string& method, const std::string& directory, handler_type handler) {
				routes_map[route_type(method, directory)] = handler;
			}

		private:
			std::map<route_type, handler_type> routes_map;
		};

    Server(io_context& io_context, std::uint16_t port)
        : io_context_(io_context)
        , acceptor_(io_context_, ip::tcp::endpoint(ip::tcp::v4(), port)) 
        , api()
	{
	}

	void start() {
		accept_connections();
		handle_connections();
		LOG(INFO) << "server started, accepting connections";
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
				Incoming_Message_type Request_Message = std::move(incoming_queue_.pop());
				// checking before formulating/sending response to not waste resources
				if (Request_Message.connection_ptr == nullptr) {
					LOG(DEBUG) << "responding to a connection that was disonected";
					continue;
				}

				LOG(DEBUG) << "connection at ip " << Request_Message.connection_ptr->get_ip()
					<< " requested " << Request_Message.http_request.method
					<< " " << Request_Message.http_request.directory;
				// make response
				std::string response = api.make_http_response(Request_Message.http_request).make_string();

				// send response
				if (Request_Message.connection_ptr == nullptr) {
					LOG(DEBUG) << "responding to a connection that was disonected";
					continue;
				}
				Request_Message.connection_ptr->write_data(response);
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

public:
	api_type api;
private:
    io_context& io_context_;
    ip::tcp::acceptor acceptor_;
    std::optional<ip::tcp::socket> new_socket_;
	std::optional<Utils::JThread> respond_thread_;
	Utils::ThreadSafeQueue<web_server::Incoming_Message_type> incoming_queue_;
};

// for the map to work bruh
bool operator<(const Server::api_type::route_type& lhs, const Server::api_type::route_type rhs) {
	return (lhs.method < rhs.method) || (lhs.method == rhs.method && lhs.directory < rhs.directory);
}

}
