// iot_monitoring_endpoint.cpp : Defines the entry point for the application.
//

#include "iot_monitoring_endpoint.h"
#include <istream>

char rn[3] = "\r\n"; //should include null termination


void test_packet() {
	iot_monitoring::data::packet<int, float> _packet;

	_packet = 1;
	_packet << 1.5f;

	std::stringstream ss;

	ss << _packet;

	
	//std::istream st = std::istream(&b);

	unsigned char* cc = new unsigned char[sizeof(int) + sizeof(float)];

	ss >> _packet;


}

struct seq_packet {
	std::vector<iot_monitoring::data::packet<uint16_t, float>> p;
	friend std::istream& operator>>(std::istream& is, seq_packet& seq) {
		
		std::istreambuf_iterator<char> beg{ is }, end;

		std::vector<char> _temp{ beg, end };

		std::vector<char>::iterator iter;
		iter = std::search(_temp.begin(), _temp.end(), rn, rn + 2);
		try {
			while (iter != _temp.end()) {
				iter += 2;

				seq.p.push_back(
					iot_monitoring::data::packet<uint16_t, float>(iter)
				);
				iter = std::search(iter, _temp.end(), rn, rn + 2);
			}
		}
		catch (...) {}//TODO: May seek after end of vector

		



		return is;
	}
};



int main(int argc, char** argv)
{
	
	iot_monitoring::arg_handler h(argc, argv);

	

	auto results = h.handle(iot_monitoring::ARGUMENTS::INSTALL);

	//iot_monitoring::serial serial("com9");

	auto ports = iot_monitoring::mc::get_available_hid();

	test_packet();

	//auto server = iot_monitoring::RemoteEndopintServer::start_server(std::launch::async,"0.0.0.0:50051");

	for (const auto& p : ports) {
		
		std::cout << "port id: " << p.first << "\n"
			<< "Vendor Id: " << p.second.dwVendorId << "\n"
			<< "Product Id: " << p.second.dwProductId << "\n"
			<< "Version Number: " << p.second.dwVersionNumber << "\n";
		std::cout << std::endl;
	}


	DCB dcb_serial_params{};
	dcb_serial_params.BaudRate = CBR_38400;
	dcb_serial_params.ByteSize = 8;
	dcb_serial_params.StopBits = ONESTOPBIT;
	dcb_serial_params.Parity = NOPARITY;
	dcb_serial_params.fDtrControl = DTR_CONTROL_ENABLE;

	

	auto id = iot_monitoring::mc::get_comm_id("USB\\VID_2341&PID_0043");
	
	auto x = iot_monitoring::mc::get_available_comm();

	std::shared_ptr<iot_monitoring::device> dev = iot_monitoring::mc::create(id, dcb_serial_params);

	iot_monitoring::serial ser(dev);

	unsigned char incomingData[256];	

	ZeroMemory(incomingData, 256 * sizeof(unsigned char));
	


	int dataLength = sizeof(int) + sizeof(float);
	int readResult = 0;


	std::stringbuf bb;
	std::istringstream istream;


	
	istream.set_rdbuf(&bb);
	iot_monitoring::data::packet<int, float> _packet;

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
		Sleep(2000);
	}

	


	return 0;
}
