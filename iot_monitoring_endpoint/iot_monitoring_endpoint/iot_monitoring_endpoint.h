// iot_monitoring_endpoint.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include "device.hpp"
#include "arg_parser.hpp"
#include "serial.hpp"
#include <thread>

#include "server.hpp"
#include "packet.hpp"

#include <future>
#include <istream>

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



HANDLE event = ::CreateEvent(
	nullptr,
	true,
	false,
	nullptr
);

std::vector<iot_monitoring::data::packet<uint16_t, float>> queue;

//Async Reading
//https://learn.microsoft.com/en-us/windows/win32/api/minwinbase/nc-minwinbase-lpoverlapped_completion_routine
VOID WINAPI CompletionRoutine(DWORD error, DWORD num, LPOVERLAPPED lpOverlapped) {
	std::cout << "Completed with total bytes: " << num << std::endl;
	std::cout << "Errors: " << error << std::endl;

	seq_packet seq;
	istream >> seq;

	std::cout << std::endl;
	std::cout << "Found " << seq.p.size() << " packets" << std::endl;
	if (seq.p.size() > 0) {
		std::cout << "Type: " << typeid(seq.p[0].header.id).name() << "\n" << "{" << seq.p[0].header.id << "," << seq.p[0].payload << "}" << std::endl;
		
		queue.insert(queue.end(), seq.p.begin(), seq.p.end());
	}
	
	
}