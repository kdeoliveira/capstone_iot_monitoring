#pragma once
#include <Windows.h>
#include <type_traits>
#include <cstring>
#include <istream>
#include <sstream>
#include <vector>
#include <iostream>

namespace iot_monitoring {
	namespace data {
		enum status {
			ON,
			OFF,
			NOT_FOUND,
			FAIL,
		};

		enum header_id : uint16_t {
			TEMP,
			CO,
			HEART,
			OXYGEN,
			GPS,
			UNKNOWN
		};

		struct device_info {
			device_info() = default;
			int id;
			std::string hardware_id;
			status hardware_status;
		};

		template<typename T>
		struct header {
			T id;
			UINT16 size;
		};

		template<typename S, typename T>
		struct packet {
			header<S> header;
			UINT16 uid;
			T payload;

			packet() = default;
			packet(std::vector<char>::iterator& beg, std::vector<char>::iterator& end) {
				
				S temp_id;
				std::memcpy((void*)&temp_id,beg._Ptr, sizeof(S));
				switch (temp_id) {
				case header_id::TEMP:
				case header_id::CO:
				case header_id::HEART:
				case header_id::OXYGEN:
				case header_id::GPS:
					header.id = temp_id;
					break;
				default:
					header.id = header_id::UNKNOWN;
					break;
				}
				
				std::advance(beg, sizeof(S));

				if (std::distance(beg, end) < sizeof(T) + sizeof(UINT16))
					throw std::exception("invalid iterator");

				std::memcpy((void*)&uid, beg._Ptr, sizeof(UINT16));

				std::advance(beg, sizeof(UINT16));

				std::memcpy((void*)&payload, beg._Ptr, sizeof(T));

				std::advance(beg , sizeof(T));

				header.size = sizeof(T) + sizeof(S);

			}

			constexpr size_t size() const {
				return header.size;
			}

			
			template<typename R> packet<S,T>& operator=(const R& value) {
				static_assert(std::is_same<R, S>::value, "invalid data type");

				S temp_id;
				std::memcpy(&temp_id, &value, sizeof(R));
				switch (temp_id) {
				case header_id::TEMP:
				case header_id::CO:
				case header_id::HEART:
				case header_id::OXYGEN:
				case header_id::GPS:
					header.id = temp_id;
					break;
				default:
					header.id = header_id::UNKNOWN;
					break;
				}

				header.size = sizeof(T) + sizeof(R);

				return *this;
			}

			packet<S, T>& operator=(header_id value) {
				std::memcpy(&header.id, &value, sizeof(header_id));
				header.size = sizeof(T) + sizeof(S);
				return *this;
			}

			template<typename R> friend packet<S, T>& operator << (packet<S, T>& data, const R& in) {
				static_assert(std::is_standard_layout<R>::value, "complex data type not allowed");

				std::memcpy(&data.payload, &in, sizeof(T));
				
				data.header.size = sizeof(T) + sizeof(R);

				return data;
			}

			friend std::ostream& operator<<(std::ostream& os, packet<S, T>& data) {
				unsigned char* _b = new unsigned char[sizeof(S) + sizeof(T)];

				S* id = (S*)_b;
				*id = data.header.id;

				os.write((char*)_b, sizeof(S));

				_b += sizeof(S);

				T* p = (T*)_b;
				*p = data.payload;
				
				os.write((char*)_b, sizeof(T));

				return os;
			}

			friend std::istream& operator>>(std::istream& st, packet<S,T>& output) {
				unsigned char* _b = new unsigned char[sizeof(T)+sizeof(S)];
				st.seekg(0);
				st.read((char*)_b, sizeof(T) + sizeof(S));
				
				std::memcpy((void*)&output.header.id, _b, sizeof(S));

				_b += sizeof(S);

				std::memcpy((void*)&output.payload, _b, sizeof(T));

				output.header.size = sizeof(T) + sizeof(S);

				return st;
			}
		};

	}
}