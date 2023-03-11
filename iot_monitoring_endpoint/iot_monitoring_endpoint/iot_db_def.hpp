#pragma once
#include <database.hpp>
#include <packet.hpp>
#include <iostream>

class SensorSchema : public iot_monitoring::database::schema<int> {
public:
	SensorSchema() : iot_monitoring::database::schema<int>(1,1){
	}
	SensorSchema(int32_t id, int32_t t) : iot_monitoring::database::schema<int>(id, t) {
	}
	virtual bsoncxx::document::value generate_document() override {
		return bsoncxx::builder::basic::make_document(
			bsoncxx::builder::basic::kvp("value", "remote_device_sensor")
		);
	}

	virtual void compile_data(bsoncxx::document::view data) override {
		//this->_data = data["value"].get_string().value;
	}
};

class HolderSchema : public iot_monitoring::database::schema<float> {
private:
	bsoncxx::oid sensor_id;
	int _uid;
public:
	HolderSchema(bsoncxx::oid sensor) : iot_monitoring::database::schema<float>(1, 1), _uid{ 0 } {
		this->sensor_id = sensor;
	}
	HolderSchema(uint64_t id, uint64_t t, int uid, bsoncxx::oid sensor) : iot_monitoring::database::schema<float>(id, t) {
		this->_uid = uid;
		this->sensor_id = sensor;
	}

	virtual bsoncxx::document::value generate_document() override {
		return bsoncxx::builder::basic::make_document(
			bsoncxx::builder::basic::kvp("uid", (int)this->_uid),
			bsoncxx::builder::basic::kvp("value", this->_data),
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

static bsoncxx::oid* sensors_id = new bsoncxx::oid[5];

void db_runner(iot_monitoring::database::store* db, std::map<uint16_t, iot_monitoring::data::PacketStream>* _ps) {
	if (_ps->empty())
		return;

	auto iter = _ps->find(0xFF);
	if (iter == _ps->end() || iter->second.empty())
		return;
	auto p = iter->second.pop();

	auto time_now = std::chrono::system_clock::now().time_since_epoch();

	iot_monitoring::database::schema<float> *doc =  new HolderSchema((int64_t)p.header.id, (int64_t)time_now.count(), p.uid, sensors_id[p.header.id]);
	doc->set_data(p.payload);
	

	db->insert_value("packet_stream", doc);

	//std::cout << "Value inserted in database\n" << "Queue qty: " << _ps->size() << std::endl;
	//bsoncxx::stdx::optional<mongocxx::result::insert_one> res = db->insert_value("sensors",doc);
	//auto x = db->get_by_id<TestSchema>(std::string("sensors"), 1);
	//iot_monitoring::database::schema<int>* holder = new HolderSchema(x->object_id);
	//db->insert_value("extra", holder);
	//auto extradb = db->get_by_id<HolderSchema>("extra", 1);
}

void initiate_database(iot_monitoring::database::store* db) {
	
	db->check_or_create_collection("packet_stream");
	auto beg = sensors_id;
	for (uint16_t x = iot_monitoring::data::header_id::TEMP; x != iot_monitoring::data::header_id::UNKNOWN; x++) {
		iot_monitoring::database::schema<int>* doc = new SensorSchema((int)x, 0);

		auto ds = db->get_by_id<SensorSchema>("sensors", (int)x);

		if (!ds) {
			auto res = db->insert_value("sensors", doc);

			*(beg++) = res->inserted_id().get_oid().value;
		}
		else {
			*(beg++) = ds->object_id;
		}

		delete doc;

		
	}

}