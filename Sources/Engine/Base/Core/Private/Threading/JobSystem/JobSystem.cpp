#include "Threading/JobSystem/JobSystem.h"
#include "Threading/JobSystem/WorkerThread.h"
#include "Threading/JobSystem/Job.h"
#include "Threading/Thread.h"
#include <sstream>

namespace ze::jobsystem
{

/** Worker list */
std::array<WorkerThread, 128> workers;
size_t worker_count = 0;

/** Job pool (one for each worker) */
thread_local std::array<Job, max_job_count_per_frame> job_pool;

/** Allocated jobs counter (one for each worker) */
thread_local std::size_t allocated_jobs = 0;

/** This worker id */
thread_local size_t worker_idx = 0;

void init_thread_local_idx()
{
	size_t idx = 0;
	for (auto& worker : workers)
	{
		if (worker.get_thread_id() == std::this_thread::get_id())
		{
			worker_idx = idx;
			break;
		}

		idx++;
	}
}

void initialize()
{
	uint32_t num_cores = std::thread::hardware_concurrency();
	uint32_t worker_thread_count = std::max<uint32_t>(1, num_cores);
	worker_count = worker_thread_count;
	
	ze::logger::info("{} hardware cores detected, spawning {} workers", 
		num_cores, worker_thread_count - 1);

	/** Add main thread */
	init_thread_local_idx();
	new (workers.data()) WorkerThread(WorkerThreadType::Partial, 
		std::this_thread::get_id());
	
	/** Workers */
	for(size_t i = 1; i < worker_thread_count; ++i)
	{
		new (workers.data() + i) WorkerThread(WorkerThreadType::Full, 
			[i]()
			{
				init_thread_local_idx();
				auto& worker = get_worker();

				/** Set a name to this thread */
				std::stringstream name;
				name << "Worker Thread " << i;
				ze::threading::set_thread_name(name.str());

				while(worker.is_active())
				{
					worker.flush();
				}
			});
	}
}

void stop()
{
	for(size_t i = 1; i < worker_count; ++i)
	{
		workers[i].stop();
	}
}

template<typename... Args>
const Job& allocate_job(Args&&... args)
{
	Job& job = job_pool[++allocated_jobs & (max_job_count_per_frame - 1)];
	job.~Job();
	new (&job) Job(std::forward<Args>(args)...);
	return job;
}

const Job& create_job(JobType type, const Job::JobFunction& job_func)
{
	return allocate_job(job_func, type);
}

const Job& create_job(JobType type, const Job::JobFunction& job_func, const Job& parent)
{
	ZE_ASSERT(parent.unfinished_jobs < Job::max_childs);
	parent.unfinished_jobs++;
	return allocate_job(job_func, type, &parent);
}

/** Getters */
WorkerThread& get_worker_by_id(const std::thread::id& thread_id)
{
	for (auto& worker : workers)
	{
		if (worker.get_thread_id() == thread_id)
			return worker;
	}

	return workers[0];
}

WorkerThread& get_worker_by_idx(const size_t& idx)
{
	return workers[idx];
}

WorkerThread& get_worker()
{
	return workers[worker_idx];
}

size_t get_worker_idx()
{
	return worker_idx;
}

size_t get_worker_count()
{ 
	return worker_count; 
}

}