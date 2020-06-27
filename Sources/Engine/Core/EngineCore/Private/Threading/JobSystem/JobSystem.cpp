#include "Threading/JobSystem/JobSystem.h"
#include "Threading/JobSystem/WorkerThread.h"
#include "Threading/JobSystem/Job.h"

DECLARE_LOG_CATEGORY(JobSystem);

namespace ZE::JobSystem
{

/** Worker list */
std::array<CWorkerThread, 32> Workers;
size_t WorkerCount = 0;

/** Job pool (one for each worker) */
thread_local std::array<SJob, GMaxJobCountPerFrame> JobPool;

/** Allocated jobs counter (one for each worker) */
thread_local std::size_t AllocatedJobs = 0;

/** This worker id */
thread_local size_t WorkerIdx = 0;

void InitThreadLocalIdx()
{
	size_t Idx = 0;
	for (auto& Worker : Workers)
	{
		if (Worker.GetThreadId() == std::this_thread::get_id())
		{
			WorkerIdx = Idx;
			break;
		}

		Idx++;
	}
}

void Initialize()
{
	uint32_t NumCores = std::thread::hardware_concurrency();
	uint32_t WorkerThreadCount = std::max<uint32_t>(1, NumCores);
	WorkerCount = WorkerThreadCount;
	
	LOG(ZE::ELogSeverity::Info, JobSystem, 
		"%d hardware cores detected, spawning %d workers (engine limit: %d) + 1 partial (main thread)", 
		NumCores, WorkerThreadCount - 1, Workers.size());

	/** Add main thread */
	InitThreadLocalIdx();
	new (Workers.data()) CWorkerThread(EWorkerThreadType::Partial, 
		std::this_thread::get_id());
	
	/** Workers */
	for(size_t i = 1; i < WorkerThreadCount; ++i)
	{
		new (Workers.data() + i) CWorkerThread(EWorkerThreadType::Full, 
			[]()
			{
				InitThreadLocalIdx();
				auto& Worker = GetWorker();

				while(Worker.IsActive())
				{
					Worker.Flush();
				}
			});
	}
}

template<typename... Args>
const SJob& AllocateJob(Args&&... InArgs)
{
	must(AllocatedJobs < GMaxJobCountPerFrame);

	SJob& Job = JobPool[++AllocatedJobs & (GMaxJobCountPerFrame - 1)];
	Job.~SJob();
	new (&Job) SJob(std::forward<Args>(InArgs)...);
	return Job;
}

const SJob& CreateJob(EJobType InType, const SJob::JobFunction& InJobFunc)
{
	return AllocateJob(InJobFunc, InType);
}

const SJob& CreateJob(EJobType InType, const SJob::JobFunction& InJobFunc, const SJob& InParent)
{
	must(InParent.UnfinishedJobs < SJob::MaxChilds)
	InParent.UnfinishedJobs++;
	return AllocateJob(InJobFunc, InType, &InParent);
}

/** Getters */
CWorkerThread& GetWorkerById(const std::thread::id& InThreadId)
{
	for (auto& Worker : Workers)
	{
		if (Worker.GetThreadId() == InThreadId)
			return Worker;
	}

	return Workers[0];
}

CWorkerThread& GetWorkerByIdx(const size_t& InIdx)
{
	return Workers[InIdx];
}

CWorkerThread& GetWorker()
{
	return Workers[WorkerIdx];
}

size_t GetWorkerCount()
{ 
	return WorkerCount; 
}


}