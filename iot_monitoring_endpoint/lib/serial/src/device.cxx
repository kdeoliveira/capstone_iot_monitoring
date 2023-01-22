#include "device.hpp"

#include <WinBase.h>

namespace iot_monitoring {
	mc::mc(std::string port, DCB serial_params, unsigned long mode)
	{
		this->_handle = ::CreateFile(port.c_str(),
				GENERIC_READ | GENERIC_WRITE,
				0,
				NULL, //shared_access
				OPEN_EXISTING,
				mode, //FILE_FLAG_OVERLAPPED for async handle
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

		DWORD ret;
		
		DCB dcb_serial_params = { 0 };

		if (GetCommState(this->_handle, &dcb_serial_params) > 0) {

			dcb_serial_params.BaudRate = serial_params.BaudRate;
			dcb_serial_params.ByteSize = serial_params.ByteSize;
			dcb_serial_params.StopBits = serial_params.StopBits;
			dcb_serial_params.Parity = serial_params.Parity;
			dcb_serial_params.fDtrControl = serial_params.fDtrControl;

			if (!SetCommState(this->_handle, &dcb_serial_params)) {
				throw std::exception("Could ont set params");

			}
			else {
				this->connected = true;
				PurgeComm(this->_handle, PURGE_RXCLEAR | PURGE_TXCLEAR);
			}

		}
		else {
			ret = GetLastError();
		}
		
	}

	mc::~mc() {
		if (this->connected) {
			this->connected = false;
			CloseHandle(this->_handle);
		}
	}

	HANDLE mc::get_handle() {
		return this->_handle;
	}


	std::vector<std::string> mc::get_available_comm() {
		std::vector<std::string> results;
		ULONG* ports = new ULONG[255];
		ZeroMemory(ports, sizeof(ULONG));

		
		ULONG num;

		ULONG res = ::GetCommPorts(ports, 255, &num);

		if (res == ERROR_SUCCESS || res == ERROR_MORE_DATA)
			for (ULONG i = 0; i < num; ++i) {
				results.push_back("COM"+std::to_string(ports[i]));
			}
		delete []ports;
		return results;
	}

	std::string mc::get_comm_id(std::string hardware_id) {
				
		//SetupDiGetClassDevs returns a handle to a device information set
		HDEVINFO device_info = SetupDiGetClassDevs(
			NULL,
			"USB",
			NULL,
			DIGCF_ALLCLASSES | DIGCF_PRESENT);
		if (device_info == INVALID_HANDLE_VALUE)
			return std::string();

		unsigned char port_name[20];
		/*DWORD required_size = sizeof(pszPortName);*/


		SP_DEVINFO_DATA device_data;
		ZeroMemory(&device_data, sizeof(SP_DEVINFO_DATA));
		device_data.cbSize = sizeof(SP_DEVINFO_DATA);
		
		DWORD index = 0;
		while (SetupDiEnumDeviceInfo(
			device_info,
			index,
			&device_data))
		{

			index++;

			DEVPROPTYPE ulPropertyType;
			DWORD Error = 0;
			unsigned char _buf[1024] = { 0 };

			//Retrieves a specified Plug and Play device property
			if (SetupDiGetDeviceRegistryProperty(device_info, &device_data, SPDRP_HARDWAREID,
				&ulPropertyType, _buf,
				sizeof(_buf),   // The size, in bytes
				NULL))
			{

				if (std::memcmp(_buf, hardware_id.c_str(), hardware_id.size()) != 0)
					continue;

				HKEY reg_key;
				//Get the key
				reg_key = SetupDiOpenDevRegKey(device_info, &device_data, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
				if (reg_key == INVALID_HANDLE_VALUE)
				{
					Error = GetLastError();
					break; //Not able to open registry
				}
				else
				{

					DWORD type;
					DWORD req_sz = sizeof(port_name);
					if ((RegQueryValueEx(reg_key, "PortName", nullptr, &type, port_name, &req_sz) == ERROR_SUCCESS) && (type == REG_SZ))
					{
						if (std::memcmp(port_name, "COM", sizeof(unsigned char) * 3) == 0) {
							return std::string(reinterpret_cast<char*>(port_name));
						}
					}
					// Close the key now that we are finished with it
					RegCloseKey(reg_key);
				}
			}
		}

		SetupDiDestroyDeviceInfoList(device_info);
		

		return std::string();

	}

	std::map<std::string, RID_DEVICE_INFO_HID> mc::get_available_hid() {

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


	
}