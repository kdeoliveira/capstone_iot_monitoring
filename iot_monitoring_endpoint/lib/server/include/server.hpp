#pragma once
#include <future>

namespace iot_monitoring {
	std::future<void> start_server(std::launch mode, const std::string& addr);
}