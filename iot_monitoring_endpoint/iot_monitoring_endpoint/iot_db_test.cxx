
#include <database.hpp>


class TestSchema : public iot_monitoring::database::schema<int> {
public:
	TestSchema() : iot_monitoring::database::schema<int>(1,1){
	}
	TestSchema(uint64_t id, uint64_t t) : iot_monitoring::database::schema<int>(id, t) {
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

int main() {
	static mongocxx::instance _inst{};

	iot_monitoring::database::store* db = new iot_monitoring::database::store("mongodb+srv://concordia:iot-monitoring@iot-monitoring.qu31apk.mongodb.net/?retryWrites=true&w=majority","iot");
	iot_monitoring::database::schema<int> *doc =  new TestSchema();

	db->check_or_create_collection("extra");

	bsoncxx::stdx::optional<mongocxx::result::insert_one> res = db->insert_value("sensors",doc);

	auto x = db->get_by_id<TestSchema>(std::string("sensors"), 1);

	
	iot_monitoring::database::schema<int>* holder = new HolderSchema(x->object_id);

	db->insert_value("extra", holder);
	auto extradb = db->get_by_id<HolderSchema>("extra", 1);

	return 0;
}