#pragma once

#include "iot_monitoring_endpoint.h"
#include "iot_db_test.hpp"
#include "polling.hpp"
#include "packet_stream.hpp"
#include <map>
#include <csignal>


static Worker* poll_db;
static Worker* poll_uart;

/// <summary>
/// Signal handler used to send a stop signal to all workers
/// It can also be used to manually trigger a stop event to those workers
/// </summary>
void handler(int sig) {
	std::cout << "Interrupt called to stop service" << std::endl;
	try {
		if(poll_db) poll_db->stop();
		if(poll_uart) poll_uart->stop();
	}
	catch (std::future_error& e) {
		std::cout << e.what() << e.code() << std::endl;
	}
}

namespace iot_monitoring {
	int main(arg_handler h) {
		//Sleep(10000);
		signal(SIGINT, handler);

		std::shared_ptr<std::map<uint16_t, iot_monitoring::data::PacketStream>> _ps = std::make_shared<std::map<uint16_t, iot_monitoring::data::PacketStream>>();
		std::shared_ptr<std::vector<iot_monitoring::data::device_info>> _ds = std::make_shared<std::vector<iot_monitoring::data::device_info>>();

		//_ps[(uint16_t)iot_monitoring::data::CO] = iot_monitoring::data::PacketStream();
		//_ps[(uint16_t)iot_monitoring::data::HEART] = iot_monitoring::data::PacketStream();
		//_ps[(uint16_t)iot_monitoring::data::TEMP] = iot_monitoring::data::PacketStream();
		//_ps[(uint16_t)iot_monitoring::data::OXYGEN] = iot_monitoring::data::PacketStream();
		//_ps[(uint16_t)iot_monitoring::data::UNKNOWN] = iot_monitoring::data::PacketStream();
		//_ps[(uint16_t)0xFF] = iot_monitoring::data::PacketStream();


		auto results = h.handle(iot_monitoring::ARGUMENTS::PORT);

		if (nullptr == results) {
			std::cout << "No port has been passed";
			return -1;
		}

		auto hardware_id = results->get_args();

		std::cout << "Attempting to connect to hardware id: " << hardware_id.front() << "\n";

		auto id = iot_monitoring::mc::get_comm_id(hardware_id.front());

		if (id.empty()) {
			std::cout << "Invalid hardware id \n";
			return -1;
		}

		/*
		* Those are standard UART parameters used for serial communication
		* Note that those values must be similar to the ones set in the IoT device; otherwise invalid data will be received
		*/

		DCB dcb_serial_params{};
		dcb_serial_params.BaudRate = CBR_38400;
		dcb_serial_params.ByteSize = 8;
		dcb_serial_params.StopBits = ONESTOPBIT;
		dcb_serial_params.Parity = NOPARITY;
		dcb_serial_params.fDtrControl = DTR_CONTROL_ENABLE;

		std::shared_ptr<iot_monitoring::device> dev = iot_monitoring::mc::create(id, dcb_serial_params, FILE_FLAG_OVERLAPPED);


		/*
			Main workers for this application are defined here
			As the application starts the inner promise should be set to a thruthy value
			This will allow the tick() function to initiate as its shared_future received a signal and maintain in the loop while(true)
		*/

		//Poll into uart serial
		//=========================
		poll_uart = new Runner(dev, _ps.get());
		auto future_uart = poll_uart->start();
		//Give some time for the IoT to start
		Sleep(2000); 
		poll_uart->set_signal(true);

		//Poll into database
		//=========================
		std::promise<bool> db_signal;
		std::shared_future<bool> done(db_signal.get_future());
		using namespace std::chrono_literals;

		poll_db = new Poll(2s);
		poll_db->set_signal(true);
		//Establish the connection with the db
		static mongocxx::instance _inst{};
		iot_monitoring::database::store* db = new iot_monitoring::database::store("mongodb+srv://concordia:iot-monitoring@iot-monitoring.qu31apk.mongodb.net/?retryWrites=true&w=majority", "iot");

		auto future_poll = poll_db->start([&]() {
			db_runner(db, _ps.get());
			});

		auto server_task = std::async(std::launch::async, [&]() {
			std::cout << "Server listening on address: " << "0.0.0.0:6501" << std::endl;

			iot_monitoring::start_server(_ds, _ps, std::launch::async, "0.0.0.0:6501");
			});


		// As all workers have successfully start let's wait for any incoming stop event
		
		future_poll.wait();
		future_uart.wait();

		//gRPC will eventually be stopped at last. This will allow any incoming/outgoing communicate to finish its execution before the server is shutdown
		// Note that in case of long-lasting communications, the stopping process may take longer to finish. Alternatively, a timeout may be set before aborting any connection
		//server_awaiter.wait_for(std::chrono::seconds(5));
		std::cout << "Shutting down server" << std::endl;
		iot_monitoring::shutdown_server();
		server_task.wait_for(std::chrono::seconds(5));

		return 0;
	}
}