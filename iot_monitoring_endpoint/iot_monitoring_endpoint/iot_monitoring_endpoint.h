// iot_monitoring_endpoint.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#include "database.hpp"
#include <map>
#include <queue>
#include <chrono>
#include <iostream>
#include "device.hpp"
#include "arg_parser.hpp"
#include "serial.hpp"
#include <thread>

#include "server.hpp"
#include "packet.hpp"

#include <future>
#include <istream>

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






char rn[3] = "\r\n"; //should include null termination


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

//Comupting of incoming data stream
std::stringbuf bb;
std::istringstream istream;




std::map<uint16_t, std::vector<iot_monitoring::data::packet<uint16_t, float>>> memory;

//Async Reading
//https://learn.microsoft.com/en-us/windows/win32/api/minwinbase/nc-minwinbase-lpoverlapped_completion_routine
//VOID WINAPI CompletionRoutine(DWORD error, DWORD num, LPOVERLAPPED lpOverlapped) {
//	std::cout << "Completed with total bytes: " << num << std::endl;
//	std::cout << "Errors: " << error << std::endl;
//
//	seq_packet seq;
//	istream >> seq;
//
//	std::cout << std::endl;
//	std::cout << "Found " << seq.p.size() << " packets" << std::endl;
//	if (seq.p.size() > 0) {
//		std::cout << "Type: " << typeid(seq.p[0].header.id).name() << "\n" << "{" << seq.p[0].header.id << "," << seq.p[0].payload << "}" << std::endl;
//		
//		try {
//			auto queue = memory.at(seq.p.front().header.id);
//			queue.insert(queue.end(), seq.p.begin(), seq.p.end());
//		}
//		catch (...) {
//			memory.insert(std::pair<uint16_t, std::vector<iot_monitoring::data::packet<uint16_t, float>>>(seq.p.front().header.id, seq.p));
//		}
//	}
//}