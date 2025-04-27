#pragma once
#include <thread>

// i couldnt figure out how to include jthreads in cmake
// so fuck it threyre not hard to write

namespace Utils {
class JThread {
public:
	JThread(std::thread&& thread)
		: thread_(std::move(thread))
	{}

	~JThread() {
		thread_.join();
	}

private:
	std::thread thread_;
};
}
