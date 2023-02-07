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
#include "packet.hpp"
#include <iostream>

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
	std::map<uint16_t, iot_monitoring::data::PacketStream>* _queue;

	std::vector<std::vector<char>::iterator> parse(std::vector<char>::iterator& iter, std::vector<char>::iterator& end) {
		std::vector<std::vector<char>::iterator> pks;
		
		do {
			if (iter[0] == 'a') {
				pks.push_back((iter));
			}
			else if (iter[0] == 'b') {
				pks.push_back((iter));
			}
			else if (iter[0] == 'c') {
				pks.push_back((iter));
			}
			else if (iter[0] == 'd') {
				pks.push_back((iter));
			}
			iter++;
		} while (iter != end && iter[0] != RN[0]);
		if (pks.size() != 4)
			throw std::exception("invalid packet");
		pks.push_back(end);
		return pks;
	}

	void tick() override {
		std::cout << "Starting serial reader" << std::endl;
		istream.set_rdbuf(&bb);

		//Async opeartion
		iot_monitoring::async_serial async_ser(_dev);

		DWORD waiter;
		async_ser.set_routine([&,this](DWORD error, DWORD num) {
			std::istreambuf_iterator<char> beg{ istream }, end;

			std::vector<char> _buffer{ beg, end };
			std::vector<char>::iterator iterator = _buffer.begin();
			
			std::cout << _buffer.data() << std::endl;
			while (iterator != _buffer.end()) {
				
				try {
					auto pks = parse(iterator, _buffer.end());
					static uint16_t uid = std::stoi(std::string(pks[0] + 1, pks[1]));
					
					for (auto x = 1; x < pks.size() - 1; ++x) {
						iot_monitoring::data::packet<uint16_t, float> packet;
						packet.uid = uid;

						if (pks[x]._Ptr[0] == 'b') {
							packet.payload = std::stof(std::string(pks[x] + 1, pks[x + 1]));
							packet = iot_monitoring::data::TEMP;
						}
						else if (pks[x]._Ptr[0] == 'c') {
							packet.payload = std::stof(std::string(pks[x] + 1, pks[x + 1]));
							packet = iot_monitoring::data::CO;
						}
						else if (pks[x]._Ptr[0] == 'd') {
							packet.payload = std::stof(std::string(pks[x] + 1, pks[x + 1]));
							packet = iot_monitoring::data::HEART;
						}

						(*_queue)[uid].push(packet);
						(*_queue)[0xFF].push(packet);

					}
					//iot_monitoring::data::packet<uint16_t,float> packet(iterator, _buffer.end());
					//(*_queue)[packet.header.id].push(packet); //queue
					//(*_queue)[0xFF].push(packet); //db
				}
				catch (std::exception&) {
					
				}
				iterator = std::search(iterator, _buffer.end(), RN, RN + 2);
				if (iterator != _buffer.end())
					iterator += 2;
			}

			
			
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
				Sleep(2100);
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

	Runner(std::shared_ptr<iot_monitoring::device> dev, std::map<uint16_t, iot_monitoring::data::PacketStream>* queue) : _dev{ dev }, _queue{queue} {}

	~Runner(){}
};