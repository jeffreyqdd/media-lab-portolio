#include "include/thread_pool.hpp"

ThreadPool::ThreadPool(size_t num_workers)
	: m_stop_flag(false) {

	m_workers.reserve(num_workers);

	for(size_t i = 0; i < num_workers; ++i) {
		m_workers.emplace_back([this] {
			while(true) {
				std::function<void()> task;

				{
					std::unique_lock<std::mutex> lock(m_queue_mutex);
					m_condition.wait(lock, [this] { return m_stop_flag || !m_tasks.empty(); });

					if(m_stop_flag && m_tasks.empty())
						return;

					task = std::move(m_tasks.front());
					m_tasks.pop();
				}

				// Execute the task
				task();
			}
		});
	}
}

ThreadPool::~ThreadPool() {
	{
		std::unique_lock<std::mutex> lock(m_queue_mutex);
		m_stop_flag = true;
	}

	m_condition.notify_all();
	for(std::thread& worker : m_workers) {
		if(worker.joinable()) {
			worker.join();
		}
	}
}
