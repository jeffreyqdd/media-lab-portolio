#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>

class ThreadPool {
public:
	explicit ThreadPool(size_t num_workers);
	~ThreadPool();

	// Submit a task to the pool
	template <typename Func, typename... Args>
	auto enqueue(Func&& func, Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>> {
		// bind the function to its args to execute it later
		using ReturnType = std::invoke_result_t<Func, Args...>;
		auto closure = std::make_shared<std::packaged_task<ReturnType()>>(
			std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
		std::future<ReturnType> result = closure->get_future();

		{
			std::unique_lock<std::mutex> lock(m_queue_mutex);
			if(m_stop_flag) {
				throw std::runtime_error("ThreadPool is stopped; cannot enqueue new tasks.");
			}
			m_tasks.emplace([closure]() { (*closure)(); });
		}

		m_condition.notify_one();
		return result;
	}

private:
	std::vector<std::thread> m_workers;
	std::queue<std::function<void()>> m_tasks;
	std::mutex m_queue_mutex;
	std::condition_variable m_condition;
	std::atomic<bool> m_stop_flag;
};
