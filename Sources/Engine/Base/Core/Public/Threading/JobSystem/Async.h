#pragma once

#include <future>
#include "JobSystem.h"

namespace ze::jobsystem
{

/**
 * std::async version using ZE jobsystem
 */
template<typename Lambda>
void async(Lambda lambda)
{
	const auto& job = create_job(
		JobType::Normal, 
		[lambda](const Job& in_job)
		{
			lambda(in_job);
		});
	schedule(job);
}

template<typename Ret, typename Lambda>
std::future<Ret> async(Lambda lambda)
{
	using TaskType = std::packaged_task<Ret(const Job&)>;

	TaskType task(lambda);
	std::future<Ret> future = task.get_future();
	const auto& job = create_job(
		JobType::Normal, 
		[task = std::move(task)](const Job& in_job)
		{
			const_cast<TaskType&>(task)(in_job);
		});
	schedule(job);
	return future;
}

}