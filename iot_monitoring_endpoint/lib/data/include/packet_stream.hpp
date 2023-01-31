#pragma once
#include <mutex>
#include <packet.hpp>
#include <queue>
#include <sstream>
#include <istream>
#include <vector>
#include <algorithm>
#include <condition_variable>
#include <any>
#include <iterator> 

#define RN "\r\n"

namespace iot_monitoring {
	namespace data {

		union data_type {
			std::string str;
			double real;
			~data_type() {}
			data_type(data_type&&) = default;
		};

		template<typename T>
		class Stream {
		protected:
			std::queue<iot_monitoring::data::packet<uint16_t, T>> _queue;
		public:
			virtual void push(std::istream&) = 0;
			virtual iot_monitoring::data::packet<uint16_t, T>& pop() = 0;
		};

		
		class PacketStream : public Stream<float> {
		private:
			std::mutex _mutex;
			std::condition_variable _cv;

		public:
			PacketStream() = default;
			PacketStream(PacketStream&&) = default;
			PacketStream(const PacketStream&) = delete;
			PacketStream* operator=(const PacketStream&) = delete;


			std::size_t size() {
				return this->_queue.size();
			}

			void push(std::istream& stream) {

				std::istreambuf_iterator<char> beg{ stream }, end;

				std::vector<char> _buffer{ beg, end };
				std::vector<char>::iterator iterator;

				

				iterator = std::search(_buffer.begin(), _buffer.end(), RN, RN + 2);

				std::unique_lock<std::mutex> lock{ this->_mutex };

				while (iterator != _buffer.end() && iterator + 2 != _buffer.end()) {
					iterator += 2;
					this->_queue.push(iot_monitoring::data::packet<uint16_t, float>(iterator));
					iterator = std::search(iterator, _buffer.end(), RN, RN + 2);
				}
			}

			iot_monitoring::data::packet<uint16_t, float>& pop() {
				std::unique_lock<std::mutex> lock{ this->_mutex };
				auto x = std::move(this->_queue.back());
				this->_queue.pop();
				return x;
			}

			iot_monitoring::data::packet<uint16_t, float>& peek() {
				return this->_queue.back();
			}

			bool empty() const {
				return this->_queue.empty();
			}

			void wait() {
				std::unique_lock<std::mutex> ul{ this->_mutex };
				this->_cv.wait(ul, [&]() {
					return !this->empty();
					});
			}



		};
	}
}