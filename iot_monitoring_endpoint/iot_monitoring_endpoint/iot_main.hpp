#pragma once

#include "iot_monitoring_endpoint.h"
#include "iot_db_test.hpp"
#include "polling.hpp"
#include "packet_stream.hpp"
#include <map>
#include <csignal>


static Worker* poll_db;
static Worker* poll_uart;


void handler(int sig) {
	std::cout << "Interrupt called to stop service" << std::endl;
	try {
		poll_db->stop();
		poll_uart->stop();
	}
	catch (std::future_error& e) {
		std::cout << e.what() << e.code() << std::endl;
	}
}

namespace iot_monitoring {
	int main(arg_handler h) {
		signal(SIGINT, handler);


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
		//=========================
		poll_uart = new Runner(dev, _ps.get());
		auto future_uart = poll_uart->start();

		Sleep(2000);
		poll_uart->set_signal(true);


		//Poll into database
		//=========================
		std::promise<bool> db_signal;
		std::shared_future<bool> done(db_signal.get_future());
		using namespace std::chrono_literals;

		poll_db = new Poll(2s);



		static mongocxx::instance _inst{};
		iot_monitoring::database::store* db = new iot_monitoring::database::store("mongodb+srv://concordia:iot-monitoring@iot-monitoring.qu31apk.mongodb.net/?retryWrites=true&w=majority", "iot");

		auto future_poll = poll_db->start([&]() {
			db_runner(db, _ps.get());
			});

		auto server_task = std::async(std::launch::async, [&]() {
			std::cout << "Server listening on address: " << "0.0.0.0:50051" << std::endl;

			iot_monitoring::start_server(_ds, _ps, std::launch::async, "0.0.0.0:50051");
			});


		
		future_uart.wait();


		//server_awaiter.wait_for(std::chrono::seconds(5));
		std::cout << "Shutting down server" << std::endl;

		iot_monitoring::shutdown_server();
		server_task.wait_for(std::chrono::seconds(5));

		return 0;
	}
}