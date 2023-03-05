// iot_monitoring_endpoint.cpp : Defines the entry point for the application.
//
#include "iot_main.hpp"
#include "installer.hpp"
#include "init.hpp"


/*
This is the main function for the iot-monitoring endpoint
The application allows the user to start this endpoint as a Windows Service or interactively in the console.

If the endpoint is installed as a windows service, a START_SERVICE signal is immediately send to the service manager to automatically start the application
*/

int __cdecl main(int argc, char** argv)
{
	h = iot_monitoring::arg_handler(argc, argv);

	//iot-monitoring starting interactively
	if (h.handle(iot_monitoring::ARGUMENTS::INTERACTIVE)) {
		std::cout << "IOT MONITORING ENDPOINT" << std::endl;
		std::cout << "\n\n";
		std::cout << "Service starting" << std::endl;
		
		iot_monitoring::main(h);

		std::cout << "\n\n";
		std::cout << "Service finishing" << std::endl;
	}
	//iot-monitoring being installed as a windows service
	else if (h.handle(iot_monitoring::ARGUMENTS::INSTALL)) {
		if (argc < 3 || !serviceInstallation()) {
			std::cout << "Service installation failed" << std::endl;
			return EXIT_FAILURE;
		}
		
		startService(argc, argv);
	}
	//no argument passed, either user didn't not provide args or service control manager has started this application
	else {
		SERVICE_TABLE_ENTRY dispatchTable[] = {
	{ SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)main_service },
			{NULL, NULL}
		};

		// call the ctrl dispatcher with the main_service entrypoint
		if (!StartServiceCtrlDispatcherA(dispatchTable)) {
			LogEvent("iot-monitoring service started");
		}

		printUsage();
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