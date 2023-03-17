#pragma once


#include <cstdint>
#include <vector>
#include <memory>
#include <bsoncxx/document/value.hpp>
#include <mongocxx/stdx.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

namespace iot_monitoring {
	namespace database {

		class schema {
		protected:
			int64_t _id;
			int64_t _timestamp;

		public:
			bsoncxx::oid object_id;

			explicit schema(int64_t id, int64_t timestamp) {
				this->_id = id;
				this->_timestamp = timestamp;
			}

			virtual void compile_data(bsoncxx::document::view) = 0;

			virtual bsoncxx::document::value generate_document() = 0;

			bsoncxx::document::value get_value() {

				return bsoncxx::builder::basic::make_document(
					bsoncxx::builder::basic::kvp(schema::id_key(), (int64_t)this->_id),
					bsoncxx::builder::basic::kvp(schema::timestamp_key(), (int64_t)this->_timestamp),
					bsoncxx::builder::basic::kvp("data", this->generate_document())
				);
			}

			void set_id(uint64_t id);
			void set_timestamp(uint64_t time);

			static const std::string id_key() {
				return "iot_id";
			}
			static const std::string timestamp_key() {
				return "iot_timestamp";
			}
		};


	}
}