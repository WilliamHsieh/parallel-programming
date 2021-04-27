#pragma once
#include <thread>
#include <queue>
#include <functional>

class ThreadPool {
  public:
	ThreadPool(int cnt) : thread_cnt(cnt) {
		auto init_thread = [this](std::stop_token tok, int i) {
			timer[i] = Clock::now();
//			auto job = std::function<void()>{};
			auto job = std::packaged_task<void()>{};

			while (!tok.stop_requested()) {
				if (auto lock = std::scoped_lock(q_m); jobs.empty()) {
					continue;
				} else {
					job = std::move(jobs.front());
					jobs.pop();
				}
				job();
			}
		};

		timer.resize(thread_cnt);
		for (int i = 0; i < thread_cnt; i++) {
			threads.emplace_back(init_thread, i);
		}
	}

	~ThreadPool() {
		for (int i = 0; i < thread_cnt; i++) {
			auto now = Clock::now();
			auto dur = std::chrono::duration<double>(now - timer[i]);
			std::cout << "Thread [" << threads[i].get_id() << "] runs " << dur.count() << "s\n";
		}
	}

	template <typename F, typename ... Args>
	requires std::invocable<F, Args...>
	auto enqueue(F&& f, Args&& ... args) {
		using res_type = typename std::invoke_result_t<F, Args...>;

//		static_assert(std::same_as<int, res_type>);
		auto task = std::packaged_task<res_type()>(std::bind(std::forward<F>(f),
												std::forward<Args>(args)...));
		auto res = task.get_future();

		auto lock = std::unique_lock(q_m);
		jobs.emplace(std::move(task)); // TODO: so dirty
//		jobs.emplace(
//			[t = std::make_shared<std::packaged_task<void()>>(std::move(task))] {
//				(*t)();
//			});
		return res;
	}

  private:
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;

	std::queue<std::packaged_task<void()>> jobs;
//	std::queue<std::function<void()>> jobs;
	std::vector<std::jthread> threads;
	std::vector<TimePoint> timer;
	std::mutex q_m;

	int thread_cnt;
};
