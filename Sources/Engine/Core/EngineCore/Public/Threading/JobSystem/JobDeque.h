#pragma once

#include <mutex>
#include <deque>

namespace ZE::JobSystem
{

struct SJob;

/**
 * A thread-safe std::deque made for the job system
 */
// TODO: Make it lock-free
class TJobDeque
{
public:
	void Push(const SJob* InElem)
	{
		std::lock_guard<std::mutex> Guard(Mutex);
		Deque.push_back(InElem);
	}

	const SJob* Pop()
	{
		std::lock_guard<std::mutex> Guard(Mutex);
		if(IsEmpty())
			return nullptr;

		const SJob* Val = Deque.front();
		Deque.pop_front();
		return Val;
	}

	/**
	 * Steal a element from the deque
	 */
	const SJob* Steal()
	{
		std::lock_guard<std::mutex> Guard(Mutex);
		if (IsEmpty())
			return nullptr;

		const SJob* Val = Deque.back();
		Deque.pop_back();
		return Val;
	}
	
	bool IsEmpty() const 
	{
		return Deque.empty();
	}

	const size_t& GetSize()
	{
		std::lock_guard<std::mutex> Guard(Mutex);
		return Deque.size();
	}
private:
	std::mutex Mutex;
	std::deque<const SJob*> Deque;
};

}