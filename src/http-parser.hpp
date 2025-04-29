#pragma once

#include <boost/asio.hpp>
#include <sstream>
#include <string>
#include <map>

namespace web_server {
namespace HTTP {

struct Request {
    std::string method;
    std::string directory;
    // looks like HTTP/1.1, i couldnt get rid of the HTTP/, but who cares
    std::string http_version;
    // map {header_name: value}
    std::map<std::string, std::string> header_map;
	std::string body;

    void SetHeaders(boost::asio::streambuf& read_buf) {
        std::istream is(&read_buf);

		std::getline(is, method, ' ');
		std::getline(is, directory, ' ');
		std::getline(is, http_version, '\r');

		std::string tmp_string;
		std::getline(is, tmp_string, '\r');

		while(tmp_string != "\n") {
			std::size_t delim_pos = tmp_string.find(": ");
			header_map[tmp_string.substr(1, delim_pos-1)] = tmp_string.substr(delim_pos+2);
			std::getline(is, tmp_string, '\r');
		}
		// for the last \n
		is.ignore(1);
    }

	void SetBody(boost::asio::streambuf& read_buf) {
		// how the fuck do you actually read from streambuf lol
		body = std::string(boost::asio::buffers_begin(read_buf.data()), boost::asio::buffers_end(read_buf.data()));
		read_buf.consume(read_buf.size());
	}

	Request()
		: method("FUCK")
	{
	}

	std::size_t GetContentLength() const {
		auto iter_content_length = header_map.find("Content-Length");
		if (iter_content_length == header_map.end()) {
			return 0;
		}
		return std::stol(iter_content_length->second);
	}

	std::string GetContentType() const {
		auto iter_content_type = header_map.find("Content-Type");
		if (iter_content_type == header_map.end()) {
			return "";
		}
		return iter_content_type->second;
	}
};

struct Response {
	std::string status;
	std::string http_version;
	std::string body;
    std::map<std::string, std::string> header_map;

	Response() 
		: status("200 OK")
		, http_version("HTTP/1.1") {
	}

	void set_status(const std::string& value) {
		status = value;
	}

	void set_content(const std::string& content_text, const std::string& content_type)
	{
		body = content_text;
		header_map["Content-Length"] = std::to_string(body.length());
		header_map["Content-Type"] = content_type.empty() ? "text" : content_type;
	}

	void set_content(std::string&& content_text, std::string&& content_type)
	{
		body = std::move(content_text);
		header_map["Content-Length"] = std::to_string(body.length());
		header_map["Content-Type"] = content_type.empty() ? "text" : std::move(content_type);
	}

	std::string make_string() {
		std::stringstream ss;

		ss << http_version << " " << status << "\r\n";
		for(const auto& it : header_map) {
			ss << it.first << ": " << it.second << "\r\n";
		}
		ss << "\r\n" << body;

		return ss.str();
	}
};

}
}
