#pragma once

#include "EngineCore.h"
#include "Job.h"
#include <thread>

namespace ze::jobsystem
{

class WorkerThread;

/**
 * Max job count that can be allocated per frame
 */
static constexpr size_t max_job_count_per_frame = 4096;

/**
 * JobSystem API
 */

/** Initialize the job system */
CORE_API void initialize();

/** Stop all worker threads */
CORE_API void stop();

/** Create a new job */
[[nodiscard]] CORE_API const Job& create_job(JobType type, 
	const Job::JobFunction& job_func);

/** Create a new child job */
[[nodiscard]] CORE_API const Job& create_job(JobType type, 
	const Job::JobFunction& job_func, const Job& parent);

/** Create a new job with user data */
template<typename T, typename... Args>
[[nodiscard]] const Job& create_job_with_userdata(JobType type, 
	const Job::JobFunction& job_func, Args&&... args)
{
	static_assert(sizeof(T) <= Job::userdata_size, "Userdata <= SJob::userdata_size (too large)");

	const Job& job = create_job(type, job_func);
	new (job.get_userdata<void*>()) T(std::forward<Args>(args)...);
	return job;
}

/** Create a new child job with user data */
template<typename T, typename... Args>
[[nodiscard]] const Job& create_child_job_with_userdata(JobType type, 
	const Job::JobFunction& job_func, 
	const Job& parent, Args&&... args)
{
	static_assert(sizeof(T) <= Job::userdata_size, "Userdata <= SJob::userdata_size (too large)");

	const Job& job = create_job(type, job_func, parent);
	new (job.get_userdata<void*>()) T(std::forward<Args>(args)...);
	return job;
}

/**
 * Create and run a job using a lambda
 * Lambda parameters must contains const SJob& InJob
 */
template<typename Lambda>
[[nodiscard]] const Job& create_job(JobType type, Lambda in_lambda)
{
	const Job& job = create_job_with_userdata<Lambda>(type,
		[](const Job& job)
		{
			Lambda& lambda = *job.get_userdata<Lambda>();
			lambda(job);
			lambda.~Lambda();
		},
		std::forward<Lambda>(in_lambda));
	return job;
}

template<typename Lambda>
[[nodiscard]] const Job& create_child_job(JobType type, const Job& parent, Lambda in_lambda)
{
	const Job& job = create_child_job_with_userdata<Lambda>(type,
		[](const Job& job)
		{
			Lambda& lambda = *job.get_userdata<Lambda>();
			lambda(job);
			lambda.~Lambda();
		}, 
		parent,
		std::forward<Lambda>(in_lambda));
	return job;
}

CORE_API WorkerThread& get_worker_by_thread_id(const std::thread::id& InThreadId);
CORE_API WorkerThread& get_worker_by_idx(const size_t& InIdx);

/*
 * Get the worker thread of the current thread
 */
CORE_API WorkerThread& get_worker();

/**
 * Get the worker thread index of the current thread
 */
CORE_API size_t get_worker_idx();
CORE_API size_t get_worker_count();
ZE_FORCEINLINE size_t get_main_worker_idx() { return 0; }

}