#pragma once

#include <future>
#include <chrono>
#include <mutex>
#include <memory>
#include <device.hpp>
#include <serial.hpp>
#include <istream>
#include <sstream>
#include "packet_stream.hpp"

class Worker {
private:
	std::mutex _mut;
protected:
	std::shared_future<bool> signal;
	std::function<void()> _fn;
	virtual void tick() = 0;

	void runner() {
		std::unique_lock<std::mutex> lock{ this->_mut };
		this->_fn();
	}
public:
	Worker(std::shared_future<bool> fn) : signal{ fn }{ }
	template<class Lambda>
	std::future<void> start(Lambda&& exec) {
		this->_fn = std::forward<Lambda>(exec);
		return std::async(std::launch::async, &Worker::tick, this);
	}

	std::future<void> start() {
		return std::async(std::launch::async, &Worker::tick, this);
	}
};

//https://codereview.stackexchange.com/questions/125579/creating-a-ticker-thread
class Poll : public Worker{
private:
	const std::chrono::seconds interval;
	
	void tick() override{
		std::future_status stats;

		do{
			stats = signal.wait_for(interval);
			if (stats == std::future_status::timeout) {
				runner();
			}
		} while (stats != std::future_status::ready);
	}

public:
	Poll(std::shared_future<bool> fn, std::chrono::seconds in) : Worker{ fn }, interval{ in }{}
};

class Runner : public Worker {
private:
	std::shared_ptr<iot_monitoring::device> _dev;
	std::map<std::string, iot_monitoring::data::PacketStream>* _queue;
	void tick() override {
		std::cout << "Starting serial reader" << std::endl;
		istream.set_rdbuf(&bb);


		//Async opeartion
		iot_monitoring::async_serial async_ser(_dev);

		DWORD waiter;
		async_ser.set_routine([&](DWORD error, DWORD num) {
			std::cout << "Completed with total bytes: " << num << std::endl;
			std::cout << "Errors: " << error << std::endl;
			std::cout << std::endl;
			(*_queue)["test"].push(istream);
			std::cout << "Total queue size: " << _queue->size() << std::endl;

			});

		signal.wait();

		while (signal.get()) {

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
public:
	Runner(std::shared_future<bool> fn, std::shared_ptr<iot_monitoring::device> dev, std::map<std::string, iot_monitoring::data::PacketStream>* queue) : Worker{ fn }, _dev{ dev }, _queue{queue} {}
};