#pragma once

#include "EngineCore.h"
#include "Job.h"
#include <thread>

namespace ZE::JobSystem
{

class CWorkerThread;

/**
 * Max job count that can be allocated per frame
 */
static constexpr size_t GMaxJobCountPerFrame = 4096;

/**
 * JobSystem API
 */

/** Initialize the job system */
ENGINECORE_API void Initialize();

/** Create a new job */
[[nodiscard]] ENGINECORE_API const SJob& CreateJob(EJobType InType, 
	const SJob::JobFunction& InJobFunc);

/** Create a new child job */
[[nodiscard]] ENGINECORE_API const SJob& CreateJob(EJobType InType, 
	const SJob::JobFunction& InJobFunc, const SJob& InParent);

/** Create a new job with user data */
template<typename T, typename... Args>
[[nodiscard]] const SJob& CreateJobUserdata(EJobType InType, 
	const SJob::JobFunction& InJobFunc, Args&&... InArgs)
{
	static_assert(sizeof(T) <= SJob::UserdataSize, "Userdata is too big !");

	const SJob& Job = CreateJob(InType, InJobFunc);
	new (Job.GetUserdata<void*>()) T(std::forward<Args>(InArgs)...);
	return Job;
}

/** Create a new child job with user data */
template<typename T, typename... Args>
[[nodiscard]] const SJob& CreateChildJobUserdata(EJobType InType, 
	const SJob::JobFunction& InJobFunc, 
	const SJob& InParent, Args&&... InArgs)
{
	static_assert(sizeof(T) <= SJob::UserdataSize, "Userdata is too big !");

	const SJob& Job = CreateJob(InType, InJobFunc, InParent);
	new (Job.GetUserdata<void*>()) T(std::forward<Args>(InArgs)...);
	return Job;
}

/**
 * Create and run a job using a lambda
 * Lambda parameters must contains const SJob& InJob
 */
template<typename Lambda>
[[nodiscard]] const SJob& CreateJob(EJobType InType, Lambda InLambda)
{
	const SJob& Job = CreateJobUserdata<Lambda>(InType,
		[](const SJob& InJob)
		{
			Lambda& InLambda = *InJob.GetUserdata<Lambda>();
			InLambda(InJob);
			InLambda.~Lambda();
		},
		std::forward<Lambda>(InLambda));
	return Job;
}

template<typename Lambda>
[[nodiscard]] const SJob& CreateChildJob(EJobType InType, SJob& InParent, Lambda InLambda)
{
	const SJob& Job = CreateChildJobUserdata<Lambda>(InType,
		[](const SJob& InJob)
	{
		Lambda& InLambda = *InJob.GetUserdata<Lambda>();
		InLambda(InJob);
		InLambda.~Lambda();
	}, 
	InParent,
	std::forward<Lambda>(InLambda));
	return Job;
}

ENGINECORE_API CWorkerThread& GetWorkerByThreadId(const std::thread::id& InThreadId);
ENGINECORE_API CWorkerThread& GetWorkerByIdx(const size_t& InIdx);
ENGINECORE_API CWorkerThread& GetWorker();
ENGINECORE_API size_t GetWorkerIdx();
ENGINECORE_API size_t GetWorkerCount();
FORCEINLINE size_t GetMainWorkerIdx() { return 0; }

}