#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <memory>
#include <mongocxx/client.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <string>


#include "schema.hpp"


namespace iot_monitoring {
	namespace database {

		//Forward declaration not possible due to member fn
		//template<typename T>
		//class schema;

		class store {
		private:
			mongocxx::database db;
			mongocxx::client client;
		public:
			store(std::string uri, std::string db_name);

			std::vector<std::string> get_collections();

			void check_or_create_collection(std::string);

			template<typename T>bsoncxx::stdx::optional<mongocxx::result::insert_one> insert_value(std::string , schema<T>* );

			template<class Class>std::unique_ptr<Class> get_by_id(std::string collection, int64_t id, mongocxx::options::find f = mongocxx::options::find{});

			template<class Class, typename K, typename V>std::unique_ptr<Class>  get_schema(std::string , std::tuple<K&&, V&&> , mongocxx::options::find  = mongocxx::options::find{});
		};

		store::store(std::string uri, std::string db_name) {
			client = mongocxx::client{ mongocxx::uri{uri} };
			this->db = client[db_name];
		}

		std::vector<std::string> store::get_collections() {
			return this->db.list_collection_names();
		}

		void store::check_or_create_collection(std::string collection) {
			if (this->db.has_collection(collection)) {
				return;
			}

			mongocxx::options::index index_options{};
			index_options.unique(true);

			this->db[collection].create_index(bsoncxx::builder::basic::make_document(
				bsoncxx::builder::basic::kvp(schema<std::nullptr_t>::id_key(), 1),
				bsoncxx::builder::basic::kvp(schema<std::nullptr_t>::timestamp_key(), 1)
			), index_options);
		}

		template<typename T>
		bsoncxx::stdx::optional<mongocxx::result::insert_one> store::insert_value(std::string collection, schema<T>* doc) {
			auto val = doc->get_value();
			auto view = val.view();
			return this->db[collection].insert_one(std::move(view));
		}

		template<class Class>
		inline std::unique_ptr<Class> store::get_by_id(std::string collection, int64_t id, mongocxx::options::find f)
		{
			static_assert(!std::is_member_function_pointer_v<decltype(&Class::id_key)>, "incorrect class");

			bsoncxx::stdx::optional<bsoncxx::document::value> result = this->db[collection].find_one(bsoncxx::builder::basic::make_document(
				bsoncxx::builder::basic::kvp(schema<std::nullptr_t>::id_key(), id)
			), f);


			std::cout << bsoncxx::to_json(result.get()) << "\n";

			if (!result) {
				return nullptr;
			}

			
			auto idkey = result.get()[schema<std::nullptr_t>::id_key()];
			auto timestamp = result.get()[schema<std::nullptr_t>::timestamp_key()];
			
			if (idkey.type() != bsoncxx::type::k_int64 && timestamp.type() != bsoncxx::type::k_int64) {
				return nullptr;
			}

			std::unique_ptr<Class> sc = std::make_unique<Class>(idkey.get_int64().value, timestamp.get_int64().value);

			sc->object_id = result.get()["_id"].get_oid().value;
			sc->compile_data(result.get()["data"].get_document());

			return sc;
		}

		template<class Class, typename K, typename V>
		inline std::unique_ptr<Class> store::get_schema(std::string collection, std::tuple<K&&, V&&> kvp, mongocxx::options::find f)
		{
			static_assert(!std::is_member_function_pointer_v<decltype(&Class::id_key)>, "incorrect class");

			bsoncxx::stdx::optional<bsoncxx::document::value> result = this->db[collection].find_one(bsoncxx::builder::basic::make_document(
				kvp
			), f);


			std::cout << bsoncxx::to_json(result.get()) << "\n";

			if (!result) {
				return nullptr;
			}


			auto idkey = result.get()[schema<std::nullptr_t>::id_key()];
			auto timestamp = result.get()[schema<std::nullptr_t>::timestamp_key()];

			if (idkey.type() != bsoncxx::type::k_int64 && timestamp.type() != bsoncxx::type::k_int64) {
				return nullptr;
			}

			std::unique_ptr<Class> sc = std::make_unique<Class>(idkey.get_int64().value, timestamp.get_int64().value);

			sc->object_id = result.get()["_id"].get_oid().value;
			sc->compile_data(result.get()["data"].get_document());

			return sc;
		}
	}
}