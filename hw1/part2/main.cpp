#include <iostream>
#include <cassert>
#include <future>
#include <random>
#include <vector>
#include "thread_pool.hpp"

// #Mutex and condition_variable
std::atomic<int> num_job1 = 0;
std::condition_variable cv;
std::mutex cv_m;

// #Jobs
void print_1() {
#ifdef debug
	std::this_thread::sleep_for(std::chrono::seconds(1));
#endif
	auto gen = std::mt19937(std::random_device{}());
	auto distrib = std::uniform_int_distribution(0, 9);
	std::cout << (distrib(gen) & 1) << std::endl;

	if (--num_job1 == 0) {
		cv.notify_all();
	}
}

struct print_2 {
	void operator()() {
		std::unique_lock<std::mutex> lock(cv_m);
		cv.wait(lock, []{ return num_job1 == 0; });
		lock.unlock();
#ifdef debug
		std::this_thread::sleep_for(std::chrono::seconds(1));
#endif
		std::cout << 2 << std::endl;
	}
};

int fib(int n) {
	if (n <= 1) return n;
	return fib(n - 1) + fib(n - 2);
}

// #Main function
int main() {
	auto pool = ThreadPool(5);
	auto result = std::vector<std::future<void>>{};
	//TODO: vim cursorline background color priority

	// send jobs
	for (int i = 0; i < 8; i++, num_job1++) {
		result.push_back(pool.enqueue(print_1));
	}
	for (int i = 0; i < 4; i++) {
		result.push_back(pool.enqueue(print_2{}));
	}

	// get result from future
	for (auto& r : result) {
		r.get();
	}
	assert(num_job1 == 0);

	// different return type(int)
	auto res = pool.enqueue(fib, 30);
	std::cout << res.get() << std::endl;

	// lambda
	auto lam = [](int x, int y) { return x + y; };
	auto res2 = pool.enqueue(lam, 3, 5);
	std::cout << res2.get() << std::endl;

	return 0;
}
