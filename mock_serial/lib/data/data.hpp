#pragma once
#include "stdint.h"

namespace iot_monitoring {
	namespace data {
		template<typename T>
		struct header {
			T id;
			uint16_t size;
		};

		template<typename S, typename T>
		struct packet {
			header<S> _header;
			uint16_t uid;
			T payload;

			constexpr size_t size() const {
				return _header.size;
			}

			
			template<typename R> packet<S,T>& operator=(const R& value) {
				//static_assert(std::is_same<R, T>::value, "invalid data type");

				memcpy(&_header.id, &value, sizeof(R));
				_header.size = sizeof(R);

				return *this;
			}

			template<typename R> friend packet<S, T>& operator << (packet<S, T>& data, const R& in) {
				//static_assert(std::is_standard_layout<R>::value, "complex data type not allowed");

				memcpy(&data.payload, &in, sizeof(R));
				data._header.size = sizeof(S) + sizeof(T);

				return data;
			}

			void serialize(char* data) {
				memcpy(data, &_header.id, sizeof(_header.id));
				memcpy(&data[sizeof(T) + 1], &payload, sizeof(payload));
			}
		};

	}
}