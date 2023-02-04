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

HANDLE event = ::CreateEvent(
	nullptr,
	true,
	true,
	nullptr
);

class Worker {
protected:
	std::mutex _mut;
	std::promise<bool> promise;
	std::shared_future<bool> signal;
	std::function<void()> _fn;
	virtual void tick() = 0;

	void runner() {
		this->_fn();
	}
public:
	Worker(){
		signal = promise.get_future();
	}
	template<class Lambda>
	std::future<void> start(Lambda&& exec) {
		this->_fn = std::forward<Lambda>(exec);
		return std::async(std::launch::async, &Worker::tick, this);
	}

	std::future<void> start() {
		return std::async(std::launch::async, &Worker::tick, this);
	}

	void stop() {
		std::unique_lock<std::mutex> lock{ this->_mut };
		this->promise = std::promise<bool>();
		this->signal = this->promise.get_future();
		this->promise.set_value(false);
	}

	void set_signal(bool val) {
		std::unique_lock<std::mutex> lock{ this->_mut };
		if(this->signal.valid())
			this->promise.set_value(val);
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
				std::unique_lock<std::mutex> lock{ this->_mut };
				runner();
			}
		} while (stats != std::future_status::ready);
	}

public:
	Poll(std::chrono::seconds in) : interval{ in }{}
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
			/*std::cout << "Completed with total bytes: " << num << std::endl;
			std::cout << "Errors: " << error << std::endl;
			std::cout << std::endl;*/
			(*_queue)["test"].push(istream);
			
			//std::cout << "Total queue size: " << _queue->size() << std::endl;
			});

		signal.wait();
		
		bool run = true;


		do {
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
				//std::cout << "Read completed" << std::endl;
				Sleep(5000);
				SetEvent(event);
				break;
			default:
				printf("WaitForSingleObjectEx (%d)\n", GetLastError());
				break;

			}
			{
				std::unique_lock<std::mutex> lock{ this->_mut };
				run = signal.get();
			}
		} while (run);
		
	}
public:
	Runner(std::shared_ptr<iot_monitoring::device> dev, std::map<std::string, iot_monitoring::data::PacketStream>* queue) : _dev{ dev }, _queue{queue} {}
};