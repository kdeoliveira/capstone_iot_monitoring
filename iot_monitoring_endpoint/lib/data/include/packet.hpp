#pragma once
#include <Windows.h>
#include <type_traits>

namespace iot_monitoring {
	namespace data {
		template<typename T>
		struct header {
			T id;
			UINT16 size;
		};

		template<typename S, typename T>
		struct packet {
			header<S> header;
			T payload;

			constexpr size_t size() const {
				return sizeof(T);
			}

			
			template<typename R> packet<S,T>& operator=(const R& value) {
				static_assert(std::is_same<R, T>::value, "invalid data type");

				std::memcpy(&header.id, &value, sizeof(R));
				header.size = sizeof(R);

				return *this;
			}

			template<typename R> friend packet<S, T>& operator << (packet<S, T>& data, const R& in) {
				static_assert(std::is_standard_layout<R>::value, "complex data type not allowed");

				std::memcpy(&data.payload, &in, sizeof(R));
				data.header.size = data.size();

				return data;
			}
		};

	}
}