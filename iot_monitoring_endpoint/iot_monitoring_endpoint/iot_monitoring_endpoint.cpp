// iot_monitoring_endpoint.cpp : Defines the entry point for the application.
//

#include "iot_monitoring_endpoint.h"
#include <future>

int main(int argc, char** argv)
{
	
	iot_monitoring::arg_handler h(argc, argv);

	

	auto results = h.handle(iot_monitoring::ARGUMENTS::INSTALL);

	//iot_monitoring::serial serial("com9");

	auto ports = iot_monitoring::hid::get_available_hid();


	auto server = iot_monitoring::RemoteEndopintServer::start_server(std::launch::async,"0.0.0.0:50051");

	for (const auto& p : ports) {
		
		std::cout << "port id: " << p.first << "\n"
			<< "Vendor Id: " << p.second.dwVendorId << "\n"
			<< "Product Id: " << p.second.dwProductId << "\n"
			<< "Version Number: " << p.second.dwVersionNumber << "\n";

		std::cout << std::endl;
		try {
			std::shared_ptr<iot_monitoring::device> dev = iot_monitoring::hid::create(p.first, DCB{});
			
			iot_monitoring::serial ser(dev->shared_from_this());

			ser.read_data(std::string("fddf"));

		}
		catch (std::exception &e) {
			std::cout << e.what();
		}

	}



	//iot_monitoring::serial serial = iot_monitoring::serial(std::string(ports));

	return 0;
}
