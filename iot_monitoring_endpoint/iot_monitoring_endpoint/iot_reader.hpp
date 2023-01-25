#pragma once
#include <memory>
#include <device.hpp>
#include <serial.hpp>
#include <istream>
#include <sstream>
#include "iot_monitoring_endpoint.h"
#include "packet_stream.hpp"

HANDLE event = ::CreateEvent(
	nullptr,
	true,
	true,
	nullptr
);

void reader(DWORD error, DWORD num) {

	
}

void producer(std::shared_ptr<iot_monitoring::device> dev, PacketStream* queue) {
	std::cout << "Starting serial reader" << std::endl;
	istream.set_rdbuf(&bb);


	//Async opeartion
	iot_monitoring::async_serial async_ser(dev);

	DWORD waiter;
	async_ser.set_routine([&](DWORD error, DWORD num) {
		std::cout << "Completed with total bytes: " << num << std::endl;
		std::cout << "Errors: " << error << std::endl;
		std::cout << std::endl;
		queue->push(istream);
		std::cout << "Total queue size: " << queue->size() << std::endl;

	});

	Sleep(2000); //Awaits arduino to awake serial


	while (true) {
		waiter = WaitForSingleObjectEx(
			event,
			INFINITE,
			TRUE //alertable state (required)
		);

		switch (waiter) {
		case 0: //pending
			ResetEvent(event);
			async_ser.read_data(bb, 255);
			break;

		case WAIT_IO_COMPLETION: //completed
			std::cout << "Read completed" << std::endl;
			Sleep(10000);
			SetEvent(event);
			break;
		default:
			printf("WaitForSingleObjectEx (%d)\n", GetLastError());
			break;

		}
	}
}