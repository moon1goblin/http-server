#pragma once

#include <string>
#include "../src/http-parser.hpp"
#include "../src/connection.hpp"

namespace api {
using web_server::HTTP::OUT_MSG_OBJ;

OUT_MSG_OBJ handle_request(const web_server::Incoming_Message_type& request) {

	std::string status = "200 OK";

	std::string body = request.parsed_msg.directory_;

	std::map<std::string, std::string> headers;
	headers["Content-Length"] = std::to_string(body.length());
	headers["Content-Type"] = "text/plain; charset=utf-8";

	OUT_MSG_OBJ return_msg(status, headers, body);
	return return_msg;
}

}
