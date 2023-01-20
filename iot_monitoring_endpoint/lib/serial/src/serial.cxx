#include "serial.hpp"



namespace iot_monitoring {
	bool serial::write_data(const std::string buf) {
		DWORD bytes_sent;
		if (!WriteFile(*this->_device->get_handle(),
			(void*)buf.c_str(),
			buf.size(),
			&bytes_sent,
			0)) {
			ClearCommError(*this->_device->get_handle(), &this->_err, &this->_status);
			return false;
		}
		else {
			return true;
		}
	}

	int serial::read_data(std::string _buf) {
		DWORD bytes_read;
		UINT to_read;

		ClearCommError(*this->_device->get_handle(), &this->_err, &this->_status);

		if (this->_status.cbInQue > 0) {
			if (this->_status.cbInQue > _buf.size()) {
				to_read = _buf.size();
			}
			else {
				to_read = this->_status.cbInQue;
			}

			char* buffer = new char[to_read];
			if (ReadFile(*this->_device->get_handle(), buffer, to_read, &bytes_read, NULL)) {
				return bytes_read;
			}
			std::cout << buffer;
		}
		return -1;
	}
}