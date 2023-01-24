#include "database.hpp"
#include "schema.hpp"


namespace iot_monitoring {
	namespace database {
		
		template<typename T>
		void schema<T>::set_data(T data) {
			this->_data = data;
		}

		template<typename T>
		void schema<T>::set_id(uint64_t id) {
			this->_id = id;
		}

		template<typename T>
		void schema<T>::set_timestamp(uint64_t time) {
			this->_timestamp = time;
		}



		
	}
}