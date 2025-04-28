#pragma once

#include <boost/asio.hpp>
#include <map>
#include <sstream>
#include <string>

namespace web_server {
namespace HTTP {

struct Request {

    Request(boost::asio::streambuf& read_buf) {
        std::istream is(&read_buf);

        std::getline(is, method, ' ');
        std::getline(is, directory, ' ');
        std::getline(is, http_version);

        std::string tmp_string;
        while(is.good()) { // NOTE: or even while(true), idk
            std::getline(is, tmp_string);
            if (tmp_string == "") {
                break;
            }
            std::size_t delim_pos = tmp_string.find(": ");
            header_map[tmp_string.substr(0, delim_pos)] = tmp_string.substr(delim_pos+2);
        }
        // no body, because we didnt read the body
		// TODO: read the fucking body
    }

    std::string method;
    std::string directory;
    // looks like HTTP/1.1, i couldnt get rid of the HTTP/, but who cares
    std::string http_version;
    // map {header_name: value}
    std::map<std::string, std::string> header_map;
};

struct Response {
	using fuck = const std::string&;
	Response(fuck status, const std::map<std::string, std::string>& headers, fuck body, fuck HTTP_version = "HTTP/1.1") 
		: status(status)
		, header_map(headers)
		, body(body)
		, HTTP_version(HTTP_version) {
	}

	Response() 
		: status("200 OK")
		, HTTP_version("HTTP/1.1") {
	}

	void set_status(const std::string& value) {
		status = value;
	}

	void set_content(const std::string& content_text, const std::string& content_type)
	{
		body = content_text;
		header_map["Content-Length"] = std::to_string(body.length());
		header_map["Content-Type"] = content_type;
	}

	std::string make_string() {
		std::stringstream ss;

		// ss << "HTTP/1.1 200 OK\r\n"
		// 	"Content-Length: 12\r\n"
		// 	"Content-Type: text/plain; charset=utf-8\r\n"
		// 	"\r\n"
		// 	"Hello World!";

		ss << HTTP_version << " " << status << "\r\n";
		for(const auto& it : header_map) {
			ss << it.first << ": " << it.second << "\r\n";
		}
		ss << "\r\n" << body;

		return ss.str();
	}

	std::string status;
	std::string HTTP_version;
	std::string body;
    std::map<std::string, std::string> header_map;
};

}
}
