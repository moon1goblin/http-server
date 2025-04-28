#pragma once

#include "boost/asio/streambuf.hpp"
#include <boost/asio.hpp>
#include <map>
#include <sstream>
#include <string>

namespace web_server {

namespace HTTP {
struct IN_MSG_OBJ {

    IN_MSG_OBJ(boost::asio::streambuf& read_buf) {
        std::istream is(&read_buf);

        std::getline(is, method_, ' ');
        std::getline(is, directory_, ' ');
        std::getline(is, http_version_);

        std::string tmp_string;
        while(is.good()) { // NOTE: or even while(true), idk
            std::getline(is, tmp_string);
            if (tmp_string == "") {
                break;
            }
            std::size_t delim_pos = tmp_string.find(": ");
            headers[tmp_string.substr(0, delim_pos)] = tmp_string.substr(delim_pos+2);
        }
        // no body, because we didnt read the body
		// TODO: read the fucking body
    }

    std::string method_;
    std::string directory_;
    // looks like HTTP/1.1, i couldnt get rid of the HTTP/, but who cares
    std::string http_version_;
    // map {header_name: value}
    std::map<std::string, std::string> headers;
};

class OUT_MSG_OBJ {
public:
	using fuck = const std::string&;
	OUT_MSG_OBJ(fuck status, const std::map<std::string, std::string>& headers, fuck body, fuck HTTP_version = "HTTP/1.1") 
		: status(status)
		, headers(headers)
		, body(body)
		, HTTP_version(HTTP_version) {
	}

	std::string MAKE_RESPONSE() {
		std::stringstream ss;

		// ss << "HTTP/1.1 200 OK\r\n"
		// 	"Content-Length: 12\r\n"
		// 	"Content-Type: text/plain; charset=utf-8\r\n"
		// 	"\r\n"
		// 	"Hello World!";

		ss << HTTP_version << " " << status << "\r\n";
		for(const auto& it : headers) {
			ss << it.first << ": " << it.second << "\r\n";
		}
		ss << "\r\n" << body;

		return ss.str();
	}

private:
	std::string status;
	std::string HTTP_version;
    std::map<std::string, std::string> headers;
	std::string body;
};

// TODO: add a body to this

}
}
