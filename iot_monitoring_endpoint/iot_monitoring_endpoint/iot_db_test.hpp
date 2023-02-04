#pragma once
#include <database.hpp>
#include <iostream>

class TestSchema : public iot_monitoring::database::schema<int> {
public:
	TestSchema() : iot_monitoring::database::schema<int>(1,1){
	}
	TestSchema(int64_t id, int64_t t) : iot_monitoring::database::schema<int>(id, t) {
	}
	virtual bsoncxx::document::value generate_document() override {
		return bsoncxx::builder::basic::make_document(
			bsoncxx::builder::basic::kvp("value", (int)this->_data)
		);
	}

	virtual void compile_data(bsoncxx::document::view data) override {
		this->_data = data["value"].get_int32().value;
	}
};

class HolderSchema : public iot_monitoring::database::schema<int> {
private:
	bsoncxx::oid sensor_id;
public:
	HolderSchema(bsoncxx::oid sensor) : iot_monitoring::database::schema<int>(1, 1) {
		this->sensor_id = sensor;
	}
	HolderSchema(uint64_t id, uint64_t t) : iot_monitoring::database::schema<int>(id, t) {
	}
	virtual bsoncxx::document::value generate_document() override {
		return bsoncxx::builder::basic::make_document(
			bsoncxx::builder::basic::kvp("value", (int)this->_data),
			bsoncxx::builder::basic::kvp("ref", bsoncxx::builder::basic::make_document(
				bsoncxx::builder::basic::kvp(
					"$ref", "sensors"
				),
				bsoncxx::builder::basic::kvp(
					"$db", "iot"
				),
				bsoncxx::builder::basic::kvp(
					"$id", this->sensor_id.to_string()
				)
			))
		);
	}

	virtual void compile_data(bsoncxx::document::view data) override {
		this->_data = data["value"].get_int32().value;
		auto id = data["ref"].get_document().view().find("$id");
		this->sensor_id = bsoncxx::oid(id->get_string().value);
	}
};

void db_runner(iot_monitoring::database::store* db, std::map<std::string, iot_monitoring::data::PacketStream>* _ps) {
	if (_ps->empty())
		return;

	auto iter = _ps->find("test");
	if (iter == _ps->end() || iter->second.empty())
		return;
	auto p = iter->second.peek();

	auto time_now = std::chrono::system_clock::now().time_since_epoch();

	iot_monitoring::database::schema<int> *doc =  new TestSchema((int64_t)p.header.id, (int64_t)time_now.count());
	doc->set_data(p.payload);

	db->check_or_create_collection("extra");

	db->insert_value("extra", doc);

	//std::cout << "Value inserted in database\n" << "Queue qty: " << _ps->size() << std::endl;

	//bsoncxx::stdx::optional<mongocxx::result::insert_one> res = db->insert_value("sensors",doc);

	//auto x = db->get_by_id<TestSchema>(std::string("sensors"), 1);

	
	//iot_monitoring::database::schema<int>* holder = new HolderSchema(x->object_id);

	//db->insert_value("extra", holder);
	//auto extradb = db->get_by_id<HolderSchema>("extra", 1);
}