#include "database.hpp"
#include "schema.hpp"


namespace iot_monitoring {
	namespace database {

		void schema::set_id(uint64_t id) {
			this->_id = id;
		}

		void schema::set_timestamp(uint64_t time) {
			this->_timestamp = time;
		}



		
	}
}