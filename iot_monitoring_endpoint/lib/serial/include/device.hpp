#pragma once

#include <Windows.h>

#include <stdlib.h>
#include <string>
#include <exception>
#include <iostream>
#include <vector>
#include <memory>
#include <iostream>
#include <map>



namespace iot_monitoring {

	class device : public std::enable_shared_from_this<device> {
	public:
		virtual HANDLE* get_handle() = 0;

	};

	class hid final : public device{
	private:
		HANDLE _handle;
		bool connected = false;
		hid(std::string,DCB);

		hid() = default;

	public:
		~hid();

		virtual HANDLE* get_handle() override;
		bool is_connected();

		[[nodiscard]] static std::shared_ptr<device> create(std::string str, DCB dcb) {
			return std::shared_ptr<hid>(new hid(str,dcb));
		}

		//https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawinputdevicelist
		//https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getrawinputdeviceinfoa
		static std::map<std::string, RID_DEVICE_INFO_HID> get_available_hid();
	};
}