#pragma once

#include <future>
#include <chrono>
#include <mutex>


//https://codereview.stackexchange.com/questions/125579/creating-a-ticker-thread
class Poll {
private:
	const std::chrono::seconds interval;
	std::shared_future<void> signal;
	std::mutex mutex;
	std::function<void()> _fn;

	void tick() {
		std::future_status stats;

		do{
			stats = signal.wait_for(interval);
			if (stats == std::future_status::timeout) {
				runner();
			}
		} while (stats != std::future_status::ready);
	}

	void runner() {
		std::lock_guard<std::mutex> l(this->mutex);
		this->_fn();
	}

public:
	Poll(std::shared_future<void> fn, std::chrono::seconds in) : signal{ fn }, interval{ in }{}

	template<class Lambda>
	std::future<void> start(Lambda&& executer) {
		this->_fn = std::forward<Lambda>(executer);
		return std::async(std::launch::async, &Poll::tick, this);
	}
};