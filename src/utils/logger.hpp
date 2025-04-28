// logger config:
#define LOGGER_CONFIG_SET_LOG_LEVEL DEBUG
#define LOGGER_CONFIG_ENABLE_TIMESTAMPS true

#pragma once
#include <iostream>
#include <chrono>
#include <format>
#include <mutex>
#include <ostream>
#include <vector>
#include <string>

enum LOG_LEVEL_TYPES {
	OFF = 0,
	ERROR = 1,
	INFO = 2,
	DEBUG = 3
};

// blank namespace so it cant be refferenced outside of file
namespace {
// for data race
std::mutex logger_mtx;

// colors for the text in terminal
const std::string color_red("\033[0;31m");
const std::string color_yellow("\033[0;33m");
const std::string color_cyan("\033[0;36m");
// const std::string white("\033[0;97m");
const std::string color_reset("\033[0m");
static const std::vector<std::string> LOG_LEVEL_COLORS {"", color_red, color_cyan, color_yellow};

// whitespace so theyre all centered
static const std::vector<std::string> LOG_LEVEL_VALUES {"", "[ERROR]", " [INFO]", "[DEBUG]"};
}

class LOG {
public:
	LOG(const LOG_LEVEL_TYPES cur_log_lvl)
		: cur_log_lvl_(cur_log_lvl)
		, log_stream_(std::cout)
	{
		if (cur_log_lvl_ > LOGGER_CONFIG_SET_LOG_LEVEL || LOGGER_CONFIG_SET_LOG_LEVEL == OFF) {
			return;
		}
		std::lock_guard<std::mutex> lock(logger_mtx);
		log_stream_ << LOG_LEVEL_COLORS[cur_log_lvl_];

		if (LOGGER_CONFIG_ENABLE_TIMESTAMPS) {
			// TODO: make this to local time
			auto now = std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now());
			log_stream_ << std::format("{0:%d.%m.%Y %H:%M:%S}", now) << " ";
		}

		log_stream_ << LOG_LEVEL_VALUES[cur_log_lvl_] << " ";
		log_stream_ << color_reset;
	}

	~LOG() {
		if (cur_log_lvl_ > LOGGER_CONFIG_SET_LOG_LEVEL) {
			return;
		}
		std::lock_guard<std::mutex> lock(logger_mtx);
		log_stream_ << std::endl;
	}

	LOG& operator<<(const auto& message) {
		std::lock_guard<std::mutex> lock(logger_mtx);

		if (cur_log_lvl_ > LOGGER_CONFIG_SET_LOG_LEVEL) {
			return *this;
		}
		log_stream_ << message;
		return *this;
	}

private:
	LOG_LEVEL_TYPES cur_log_lvl_;
	std::ostream& log_stream_;
};
