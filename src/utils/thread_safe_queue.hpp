#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>

namespace Utils {

template <typename data_type>
class ThreadSafeQueue {
public:
	ThreadSafeQueue()
		: queue_()
		, mtx_()
		, is_not_empty()
	{}

	void push(const data_type& value) {
		std::unique_lock<std::mutex> lock(mtx_);
		queue_.push(value);
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

}
