#include "serial.hpp"



namespace iot_monitoring {
	bool serial::write_data(const std::string buf) {
		DWORD bytes_sent;
		if (!WriteFile(this->_device->get_handle(),
			(void*)buf.c_str(),
			buf.size(),
			&bytes_sent,
			0)) {
			ClearCommError(this->_device->get_handle(), &this->_err, &this->_status);
			return false;
		}
		else {
			return true;
		}
	}

    int serial::read_data(std::stringbuf& sb, std::size_t nbChar) {
        //Number of bytes we'll have read
        DWORD bytesRead;
        //Number of bytes we'll really ask to read
        unsigned int toRead;

        unsigned char* _buf = new unsigned char[nbChar];
        
        
        ClearCommError(this->_device->get_handle(), &this->_err, &this->_status);

        //Check if there is something to read
        if (this->_status.cbInQue > 0)
        {
            //If there is we check if there is enough data to read the required number
            //of characters, if not we'll read only the available characters to prevent
            //locking of the application.
            if (this->_status.cbInQue > nbChar)
            {
                toRead = nbChar;
            }
            else
            {
                toRead = this->_status.cbInQue;
            }

            //Try to read the require number of chars, and return the number of read bytes on success
            if (ReadFile(this->_device->get_handle(), _buf, toRead, &bytesRead, NULL))
            {
                sb.sputn((char*)_buf, (std::size_t)bytesRead);
                return bytesRead;
            }

        }
        
        return 0;
	}

    void async_serial::set_routine(LPOVERLAPPED_COMPLETION_ROUTINE routine) {
        this->_compl_routine = routine;
    }

    bool async_serial::write_data(const std::string buf) {
        DWORD bytes_sent;
        if (!WriteFile(this->_device->get_handle(),
            (void*)buf.c_str(),
            buf.size(),
            &bytes_sent,
            0)) {
            ClearCommError(this->_device->get_handle(), &this->_err, &this->_status);
            return false;
        }
        else {
            return true;
        }
    }

    int async_serial::read_data(std::stringbuf& sb, std::size_t nbChar) {

        OVERLAPPED overlapped = {0};
        
        //Number of bytes we'll have read
        DWORD bytesRead;
        //Number of bytes we'll really ask to read
        unsigned int toRead;

        unsigned char* _buf = new unsigned char[nbChar];

        


        ClearCommError(this->_device->get_handle(), &this->_err, &this->_status);

        //Check if there is something to read
        if (this->_status.cbInQue > 0)
        {
            //If there is we check if there is enough data to read the required number
            //of characters, if not we'll read only the available characters to prevent
            //locking of the application.
            if (this->_status.cbInQue > nbChar)
            {
                toRead = nbChar;
            }
            else
            {
                toRead = this->_status.cbInQue;
            }

            //Try to read the require number of chars, and return the number of read bytes on success
            if (ReadFileEx(this->_device->get_handle(), _buf, toRead, &overlapped, this->_compl_routine) > 0)
            {
                if (GetLastError() != ERROR_INVALID_USER_BUFFER || GetLastError() != ERROR_NOT_ENOUGH_MEMORY) {
                    sb.sputn((char*)_buf, (std::size_t)toRead); //Stores a pointer to the async returning buffer into stirngbuffer instead
                    return 0;
                }
                else {
                    throw std::exception("Invalid buffer or memory");
                }
            }
            else {
                printf("%d", GetLastError());
            }

        }

        return 0;
    }
}