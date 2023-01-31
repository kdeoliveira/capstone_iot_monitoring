// iot_monitoring_endpoint.cpp : Defines the entry point for the application.
//
#include "iot_main.hpp"



int main(int argc, char** argv)
{

	std::cout << "IOT MONITORING ENDPOINT" << std::endl;
	std::cout << "\n\n";
	std::cout << "Service starting" << std::endl;
	iot_monitoring::arg_handler h(argc, argv);

	iot_monitoring::main(h);

	std::cout << "\n\n";
	std::cout << "Service finishing" << std::endl;

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