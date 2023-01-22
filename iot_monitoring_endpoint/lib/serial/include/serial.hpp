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

		int read_data(std::stringbuf&, std::size_t);
		bool write_data(const std::string);
	};

	class async_serial {
	private:
		COMSTAT _status;
		DWORD _err;
		std::shared_ptr<device> _device;
		LPOVERLAPPED_COMPLETION_ROUTINE _compl_routine; //VOID WINAPI
	public:
		async_serial(std::shared_ptr<device> dev) {
			_device = dev;
		}

		void set_routine(LPOVERLAPPED_COMPLETION_ROUTINE);

		int read_data(std::stringbuf&, std::size_t);
		bool write_data(const std::string);
	};
}