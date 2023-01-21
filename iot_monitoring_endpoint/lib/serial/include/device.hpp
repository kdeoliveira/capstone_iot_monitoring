#pragma once
#pragma comment (lib, "OneCore.lib")


#include <Windows.h>

#include <WinBase.h>

#include <stdlib.h>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <vector>
#include <memory>
#include <iostream>
#include <map>
#include <setupapi.h>


namespace iot_monitoring {

	class device : public std::enable_shared_from_this<device> {
	protected:
		bool connected = false;
	public:
		virtual HANDLE get_handle() = 0;
		bool is_connected() {
			return this->connected;
		}
	};

	class mc final : public device{
	private:
		mc(std::string,DCB);
		mc() = default;

	protected:
		HANDLE _handle;
	public:
		~mc();
		virtual HANDLE get_handle() override;
		

		[[nodiscard]] static std::shared_ptr<device> create(std::string str, DCB dcb) {
			return std::shared_ptr<mc>(new mc(str,dcb));
		}

		//https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawinputdevicelist
		//https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getrawinputdeviceinfoa
		static std::map<std::string, RID_DEVICE_INFO_HID> get_available_hid();
		static std::vector<std::string> get_available_comm();
		static std::string get_comm_id(std::string);
		


	};
}