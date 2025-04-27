#pragma once

#include <boost/asio.hpp>
#include <map>
#include <sstream>
#include <string>

namespace web_server {

struct HTTP_Parser {
    HTTP_Parser(const std::string& message) {
        std::stringstream strstr(message);

        std::getline(strstr, method_, ' ');
        std::getline(strstr, directory_, ' ');
        std::getline(strstr, http_version_);

        std::string tmp_string;
        while(strstr.good()) { // NOTE: or even while(true), idk
            std::getline(strstr, tmp_string);
            if (tmp_string == "") {
                break;
            }
            std::size_t delim_pos = tmp_string.find(": ");
            headers[tmp_string.substr(0, delim_pos)] = tmp_string.substr(delim_pos+2);
        }
        // no body, because we didnt read the body
    }
    std::string method_;
    std::string directory_;
    // looks like HTTP/1.1, i couldnt get rid of the HTTP/, but who cares
    std::string http_version_;
    // map {header_name: value}
    std::map<std::string, std::string> headers;
};

}
