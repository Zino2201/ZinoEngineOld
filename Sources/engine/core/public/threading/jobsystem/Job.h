#pragma once

#include "EngineCore.h"
#include <atomic>
#include <array>
#include <memory>

namespace ze::jobsystem
{

#if __cpp_lib_hardware_interference_size >= 201603
static constexpr size_t job_alignement = std::hardware_destructive_interference_size;
#else
static constexpr size_t job_alignement = 64;
#endif

enum class JobType
{
	/** 
	 * The default job type,
	 * Jobs of this type will always be queue to a worker thread to not block any execution
	 */
	Normal,

	/**
	 * Mark this job has lightweight
	 * Allow this job to be directly executed
	 * If the job has any dependencies, it will also be directly executed instead to be enqueued
	 */
	Lightweight,
};

/**
 * Structure that represents a job
 * You can create jobs using ZE::JobSystem::CreateJob, ZE::JobSystem::CreateChildJob,
 *	ZE::JobSystem::CreateJobUserdata
 * To schedule a job, use ZE::JobSystem::ScheduleJob
 * You can wait for a job using ZE::JobSystem::WaitJob
 */
struct CORE_API alignas(job_alignement) Job
{
	using JobFunction = void(*)(const Job& job);
	static constexpr size_t userdata_size = 128;
	static constexpr size_t max_dependents = 16;
	static constexpr uint8_t max_childs = 255;

	JobType type;
	const Job* parent;
	/** Unfinished job count, 0 = finished, 1 = not finished, > 1 = childs running */
	mutable std::atomic_uint8_t unfinished_jobs;
	const JobFunction function;
	mutable std::atomic_uint8_t dependances_count;
	mutable std::atomic_uint8_t dependent_count;
	mutable std::array<const Job*, max_dependents> dependents;
	mutable std::array<uint8_t, userdata_size> userdata;

	Job() : type(JobType::Normal), parent(nullptr), unfinished_jobs(0), function(nullptr) {}
	Job(const JobFunction& in_job_func, JobType in_type = JobType::Normal,
		const Job* in_parent = nullptr) : 
		type(in_type), parent(in_parent),
		unfinished_jobs(1), function(in_job_func), dependances_count(0),
		dependent_count(0) {}
		 
	/**
	 * Get casted user data
	 */
	template<typename T>
	ZE_FORCEINLINE T* get_userdata() const
	{
		return reinterpret_cast<T*>(userdata.data());
	}

	ZE_FORCEINLINE bool is_finished() const { return unfinished_jobs == 0; }
};

/** Schedule the job */
CORE_API void schedule(const Job& job);

/** Schedule the job after the specified job finish */
CORE_API void schedule(const Job& job, const Job& dependence);

/** Wait for a job */
CORE_API void wait(const Job& job);

namespace detail
{
	/**
	 * Execute the job
	 */
	CORE_API void execute(const Job& InJob);

	/**
	 * Finish the job
	 * If this job has any childs, it will wait
	 * If this job has dependents, it will execute them
	 */
	CORE_API void finish(const Job& InJob);
}


}