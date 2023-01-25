#pragma once

#include <memory>
#include <Windows.h>
#include "device.hpp"
#include <iostream>
#include <sstream>
#include <functional>

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
		std::function<void(DWORD, DWORD)> _compl_routine; //VOID WINAPI
	public:
		async_serial(std::shared_ptr<device> dev) {
			_device = dev;
		}

		void set_routine(std::function<void(DWORD, DWORD)>);

		int read_data(std::stringbuf&, std::size_t);
		bool write_data(const std::string);

	};

	//Order is important since first member that will be retrieved via pointer will be overlapped
	struct read_context {
		OVERLAPPED _overlapped;
		std::function<void(DWORD, DWORD)>CompletionRoutine;
	};


}