#pragma once

#include <future>
#include "JobSystem.h"

namespace ZE
{

/**
 * std::async version using ZE jobsystem
 */
template<typename Ret, typename Lambda>
std::future<Ret> Async(Lambda InLambda)
{
	using TaskType = std::packaged_task<Ret(const ZE::JobSystem::SJob& InJob)>;

	TaskType Task(InLambda);
	std::future<Ret> Future = Task.get_future();
	const auto& Job = ZE::JobSystem::CreateJob(ZE::JobSystem::EJobType::Normal, 
		[Task = std::move(Task)](const ZE::JobSystem::SJob& InJob)
		{
			const_cast<TaskType&>(Task)(InJob);
		});
	ZE::JobSystem::ScheduleJob(Job);
	return Future;
}

}