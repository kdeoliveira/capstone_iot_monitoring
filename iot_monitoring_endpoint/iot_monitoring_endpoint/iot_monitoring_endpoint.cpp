// iot_monitoring_endpoint.cpp : Defines the entry point for the application.
//
#include "iot_main.hpp"
#include "installer.hpp"
#include "init.hpp"



int __cdecl main(int argc, char** argv)
{

	
	
	h = iot_monitoring::arg_handler(argc, argv);

	if (h.handle(iot_monitoring::ARGUMENTS::INTERACTIVE)) {
		std::cout << "IOT MONITORING ENDPOINT" << std::endl;
		std::cout << "\n\n";
		std::cout << "Service starting" << std::endl;
		
		iot_monitoring::main(h);

		std::cout << "\n\n";
		std::cout << "Service finishing" << std::endl;
	}
	else if (h.handle(iot_monitoring::ARGUMENTS::INSTALL)) {
		serviceInstallation(argc, argv);
	}
	else {
		SERVICE_TABLE_ENTRY dispatchTable[] = {
	{ "iot-monitoring", (LPSERVICE_MAIN_FUNCTION)main_service },
			{NULL, NULL}
		};

		if (!StartServiceCtrlDispatcherA(dispatchTable)) {
			LogEvent("iot-monitoring service started");
		}
	}
	

	delete poll_uart;
	delete poll_db;
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