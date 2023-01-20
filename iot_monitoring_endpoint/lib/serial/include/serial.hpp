#pragma once

#include <memory>
#include <Windows.h>
#include "device.hpp"

namespace iot_monitoring {
	class serial {
	private:
		COMSTAT _status;
		DWORD _err;
		std::shared_ptr<device> _device;

	public:
		serial(std::shared_ptr<device> dev) {
			_device = dev;
		}
		int read_data(std::string);
		bool write_data(const std::string);
	};
}