// iot_monitoring_endpoint.cpp : Defines the entry point for the application.
//

#include "iot_monitoring_endpoint.h"
#include "iot_reader.hpp"
#include "iot_db_test.hpp"
#include "polling.hpp"
#include "packet_stream.hpp"



int main(int argc, char** argv)
{

	std::cout << "IOT MONITORING ENDPOINT" << std::endl;
	std::cout << "\n\n";

	iot_monitoring::arg_handler h(argc, argv);

	PacketStream* _ps = new PacketStream();

	auto results = h.handle(iot_monitoring::ARGUMENTS::PORT);

	auto hardware_id = results->get_args();
	
	std::cout << "Attempting to connect to hardware id: " << hardware_id.front() << "\n";

	auto id = iot_monitoring::mc::get_comm_id(hardware_id.front());

	if (id.empty()) {
		std::cout << "Invalid hardware id \n";
		return -1;
	}

	DCB dcb_serial_params{};
	dcb_serial_params.BaudRate = CBR_38400;
	dcb_serial_params.ByteSize = 8;
	dcb_serial_params.StopBits = ONESTOPBIT;
	dcb_serial_params.Parity = NOPARITY;
	dcb_serial_params.fDtrControl = DTR_CONTROL_ENABLE;

	std::shared_ptr<iot_monitoring::device> dev = iot_monitoring::mc::create(id, dcb_serial_params, FILE_FLAG_OVERLAPPED);


	std::thread thread_reader{ std::move(producer), dev, _ps };

	
	

	auto server_awaiter = iot_monitoring::start_server(std::launch::async, "0.0.0.0:50055");
	
	//Poll into database
	std::promise<void> sign;
	std::shared_future<void> done(sign.get_future());
	using namespace std::chrono_literals;

	Poll poll_db(done, 2s);

	static mongocxx::instance _inst{};
	iot_monitoring::database::store* db = new iot_monitoring::database::store("mongodb+srv://concordia:iot-monitoring@iot-monitoring.qu31apk.mongodb.net/?retryWrites=true&w=majority", "iot");

	auto future_poll = poll_db.start([&]() {
		db_runner(db, _ps);
	});
	
	if (thread_reader.joinable())
		thread_reader.join();

	server_awaiter.wait_for(std::chrono::seconds(5));

	sign.set_value();

	//Sync operation
	/*iot_monitoring::serial ser(dev);

	//Receive rate of 1 packet/sec
	while (dev->is_connected()) {
		if (ser.read_data(bb, 255) > 0) {
			
			seq_packet seq;
			istream >> seq;
			
			std::cout << std::endl;
			std::cout << "Found " << seq.p.size() << " packets" << std::endl;
			if (seq.p.size() > 0) {
				std::cout << "Type: " << typeid(seq.p[0].header.id).name() << "\n" << "{" << seq.p[0].header.id << "," << seq.p[0].payload << "}" << std::endl;
			}

			
		}
		Sleep(10000);
	}*/

	


	return 0;
}
