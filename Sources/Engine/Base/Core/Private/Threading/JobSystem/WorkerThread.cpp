#include "Threading/JobSystem/WorkerThread.h"
#include "Threading/JobSystem/JobSystem.h"
#include <random>

namespace ze::jobsystem
{

/** Global condition_variable for sleeping workers */
std::condition_variable sleep_condition_var;

std::condition_variable& WorkerThread::get_sleep_condition_var() { return sleep_condition_var; }

WorkerThread::WorkerThread() : type(WorkerThreadType::Full), active(false) {}
WorkerThread::WorkerThread(WorkerThreadType type,
	const std::thread::id& in_thread_id) : active(true), type(type), thread_id(in_thread_id) {}

const Job* WorkerThread::try_get_or_steal_job(const size_t& in_worker_idx)
{
	const Job* job = job_queue.pop();
	if(!job && type != WorkerThreadType::Partial)
	{
		/** 
		 * Try to steal it from another worker
		 */
		std::random_device device;
		std::mt19937 gen(device());
		std::uniform_int_distribution<size_t> distribution(0, get_worker_count());

		size_t worker_idx = in_worker_idx == -1 ? distribution(gen) : in_worker_idx;
		auto& worker_to_steal = get_worker_by_idx(worker_idx);
		
		/**
		 * Test if it isn't this worker
		 */
		if(*this != worker_to_steal)
		{
			const Job* job = worker_to_steal.job_queue.steal();
			if(job)
				return job;
		}

		std::this_thread::yield();
		return nullptr;
	}

	return job;
}

void WorkerThread::flush()
{
	/** Try steal from the main worker, if no job found try again */
	const Job* job = try_get_or_steal_job(get_main_worker_idx());
	if (job)
	{
		detail::execute(*job);
	}
	else
	{
		const Job* job = try_get_or_steal_job();
		if(job)
		{
			detail::execute(*job);
		}
		else
		{
			if (type != WorkerThreadType::Partial)
			{
				std::unique_lock<std::mutex> lock(sleep_mutex);
				sleep_condition_var.wait(lock);
			}
		}
	}
}

}