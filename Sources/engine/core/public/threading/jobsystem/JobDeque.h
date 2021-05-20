#pragma once

#include <mutex>
#include <deque>

namespace ze::jobsystem
{

struct Job;

/**
 * A thread-safe std::deque made for the job system
 */
// TODO: Make it lock-free
class JobDeque
{
public:
	void push(const Job* elem)
	{
		std::lock_guard<std::mutex> guard(mutex);
		deque.push_back(elem);
	}

	const Job* pop()
	{
		std::lock_guard<std::mutex> guard(mutex);
		if(is_empty())
			return nullptr;

		const Job* val = deque.front();
		deque.pop_front();
		return val;
	}

	/**
	 * Steal a element from the deque
	 */
	const Job* steal()
	{
		std::lock_guard<std::mutex> guard(mutex);
		if (is_empty())
			return nullptr;

		const Job* val = deque.back();
		deque.pop_back();
		return val;
	}
	
	ZE_FORCEINLINE bool is_empty() const 
	{
		return deque.empty();
	}

	size_t get_size()
	{
		std::lock_guard<std::mutex> guard(mutex);
		return deque.size();
	}
private:
	std::mutex mutex;
	std::deque<const Job*> deque;
};

}