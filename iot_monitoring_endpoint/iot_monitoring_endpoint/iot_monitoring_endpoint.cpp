// iot_monitoring_endpoint.cpp : Defines the entry point for the application.
//

#include "iot_monitoring_endpoint.h"
#include "iot_reader.hpp"
#include "iot_db_test.hpp"
#include "polling.hpp"
#include "packet_stream.hpp"
#include <map>


int main(int argc, char** argv)
{

	std::cout << "IOT MONITORING ENDPOINT" << std::endl;
	std::cout << "\n\n";

	iot_monitoring::arg_handler h(argc, argv);

	std::shared_ptr<std::map<std::string, iot_monitoring::data::PacketStream>> _ps = std::make_shared<std::map<std::string, iot_monitoring::data::PacketStream>>();

	std::shared_ptr<std::vector<iot_monitoring::data::device_info>> _ds = std::make_shared<std::vector<iot_monitoring::data::device_info>>();


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



	
	//Poll into uart serial
	std::promise<bool> serial_signal;
	std::shared_future<bool> serial_done(serial_signal.get_future());

	Runner poll_uart(serial_done, dev, _ps.get());
	
	auto future_uart = poll_uart.start();
	
	Sleep(2000);
	serial_signal.set_value(true);


	//Poll into database
	std::promise<bool> sign;
	std::shared_future<bool> done(sign.get_future());
	using namespace std::chrono_literals;

	Poll poll_db(done, 2s);


	static mongocxx::instance _inst{};
	iot_monitoring::database::store* db = new iot_monitoring::database::store("mongodb+srv://concordia:iot-monitoring@iot-monitoring.qu31apk.mongodb.net/?retryWrites=true&w=majority", "iot");

	auto future_poll = poll_db.start([&]() {
		db_runner(db, _ps.get());
	});
	
	auto server_awaiter = iot_monitoring::start_server(_ds, _ps, std::launch::async, "0.0.0.0:50051");

	std::cout << "Service started" << std::endl;
	future_uart.wait();


	server_awaiter.wait_for(std::chrono::seconds(5));

	sign.set_value(true);

	std::cout << "Service finished" << std::endl;
	return 0;
}
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