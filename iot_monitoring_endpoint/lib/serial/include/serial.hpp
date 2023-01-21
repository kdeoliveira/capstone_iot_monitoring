#pragma once

#include <memory>
#include <Windows.h>
#include "device.hpp"
#include <iostream>
#include <sstream>

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

        serial() = default;

		int serial::read_data(std::stringbuf&, std::size_t);
		bool write_data(const std::string);

	};
}