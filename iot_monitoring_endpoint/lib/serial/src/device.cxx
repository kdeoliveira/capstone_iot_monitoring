#include "device.hpp"

namespace iot_monitoring {
	hid::hid(std::string port, DCB serial_params)
	{
		this->_handle = ::CreateFile(port.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL, //shared_access
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		



		if (this->_handle == INVALID_HANDLE_VALUE) {
			if (GetLastError() == ERROR_FILE_NOT_FOUND) {
				
				throw std::exception();

			}
			else {
				throw std::exception("error");
			}
		}

		
		
		DCB dcb_serial_params = { 0 };

		if (!GetCommState(this->_handle, &dcb_serial_params)) {
			/*dcb_serial_params.BaudRate = CBR_9600;
			dcb_serial_params.ByteSize = 8;
			dcb_serial_params.StopBits = ONESTOPBIT;
			dcb_serial_params.Parity = NOPARITY;
			dcb_serial_params.fDtrControl = DTR_CONTROL_ENABLE;*/

			dcb_serial_params.BaudRate = serial_params.BaudRate;
			dcb_serial_params.ByteSize = serial_params.ByteSize;
			dcb_serial_params.StopBits = serial_params.StopBits;
			dcb_serial_params.Parity = serial_params.Parity;
			dcb_serial_params.fDtrControl = serial_params.fDtrControl;

			if (!SetCommState(this->_handle, &dcb_serial_params)) {
				//throw std::exception("Could ont set params");

			}
			else {
				this->connected = true;
				PurgeComm(this->_handle, PURGE_RXCLEAR | PURGE_TXCLEAR);
			}

		}
		
	}

	hid::~hid() {
		if (this->connected) {
			this->connected = false;
			CloseHandle(this->_handle);
		}
	}

	bool hid::is_connected() {
		return this->connected;
	}

	std::map<std::string, RID_DEVICE_INFO_HID> hid::get_available_hid() {

		std::map<std::string, RID_DEVICE_INFO_HID> devices = std::map<std::string, RID_DEVICE_INFO_HID>();

		UINT* nDevices = new UINT;
		RAWINPUTDEVICELIST* pRawInputDeviceList;

		if (GetRawInputDeviceList(
			NULL,
			nDevices,
			sizeof(RAWINPUTDEVICELIST)
		) != (UINT)0) {
			throw std::exception("Unable to find devices");
		}

		if (nDevices == 0) { return devices; }

		pRawInputDeviceList = new RAWINPUTDEVICELIST[sizeof(RAWINPUTDEVICELIST) * *nDevices];

		*nDevices = GetRawInputDeviceList(
			pRawInputDeviceList,
			nDevices,
			sizeof(RAWINPUTDEVICELIST));

		if (*nDevices == (UINT)-1) {
			if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
				throw std::exception("insufficient buffer");
			}
		}

		for (UINT i = 0; i < *nDevices; ++i) {
			if (pRawInputDeviceList[i].dwType == RIM_TYPEHID) {
				char* device_name;


				UINT* sz = new UINT;
				if (GetRawInputDeviceInfo(
					pRawInputDeviceList[i].hDevice,
					RIDI_DEVICENAME,
					NULL,
					sz
				) != (UINT)-1) {
					device_name = new char[sizeof(char) * *sz];

					GetRawInputDeviceInfo(
						pRawInputDeviceList[i].hDevice,
						RIDI_DEVICENAME,
						device_name,
						sz
					);

					RID_DEVICE_INFO devInfo;
					devInfo.cbSize = sizeof(devInfo);
					*sz = sizeof(devInfo);

					GetRawInputDeviceInfo(
						pRawInputDeviceList[i].hDevice,
						RIDI_DEVICEINFO,
						&devInfo,
						sz
					);

					devices[device_name] = devInfo.hid;

				}
				else {
					continue;
				}

			}
		}


		return devices;
	}

	HANDLE* hid::get_handle(){
		return &this->_handle;
	}

	
}