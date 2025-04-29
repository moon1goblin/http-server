#pragma once

#include "http-parser.hpp"

namespace web_server {
class api{ 
public:
	using handler_type = std::function<void(const HTTP::Request&, HTTP::Response&)>;

	// has a custom < for the map to work, see the end of this file
	struct route_type {
		std::string method;
		std::string directory;
	};

	HTTP::Response make_http_response(const HTTP::Request& Request) {
		auto iter_route = routes_map_.find(route_type(Request.method, Request.directory));

		// default not found page
		if (iter_route == routes_map_.end()) {
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
		routes_map_[route_type(method, directory)] = handler;
	}

	void add_route(std::string&& method, std::string&& directory, handler_type handler) {
		routes_map_[route_type(std::move(method), std::move(directory))] = handler;
	}

private:
	std::map<route_type, handler_type> routes_map_;
};

// for the map to work
bool operator<(const api::route_type& lhs, const api::route_type rhs) {
	return (lhs.method < rhs.method) || (lhs.method == rhs.method && lhs.directory < rhs.directory);
}

}
