#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>

#include "http-parser.hpp"

namespace web_server {
// forward declaration
class Connection;

namespace connection_manager {

template <typename data_type>
class ThreadSafeQueue {
public:
	void push(const data_type& value) {
		std::unique_lock<std::mutex> lock(mtx_);
		queue_.push(value);
		is_not_empty.notify_one();
	}

	void push(data_type&& value) {
		std::unique_lock<std::mutex> lock(mtx_);
		queue_.push(std::move(value));
		is_not_empty.notify_one();
	}

	// waits for data to appear, blocking
	data_type pop() {
		std::unique_lock<std::mutex> lock(mtx_);
		while(queue_.empty()) {
			is_not_empty.wait(lock);
		}

		data_type popped_value = queue_.front();
		queue_.pop();

		return popped_value;
	}

private:
	std::queue<data_type> queue_;
	std::mutex mtx_;
	std::condition_variable is_not_empty;
};

struct Incoming_Message_type {
	web_server::HTTP::Request http_request;
	std::shared_ptr<Connection> connection_ptr;

	Incoming_Message_type(web_server::HTTP::Request&& http_request
			, std::shared_ptr<Connection> connection_ptr) 
		: http_request(std::move(http_request))
		, connection_ptr(connection_ptr) 
	{
	}

	Incoming_Message_type(const web_server::HTTP::Request& http_request
			, std::shared_ptr<Connection> connection_ptr) 
		: http_request(http_request)
		, connection_ptr(connection_ptr) 
	{
	}
};

}
}
