#pragma once

#include <future>
#include <string>
#include <packet.hpp>
#include <packet_stream.hpp>
#include <functional>
#include <map>

namespace iot_monitoring {
	void start_server(std::shared_ptr<std::vector<data::device_info>>, std::shared_ptr<std::map<uint16_t, data::PacketStream>>,std::launch, const std::string&);
	
	void shutdown_server();
}