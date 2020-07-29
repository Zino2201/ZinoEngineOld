#include "Threading/JobSystem/WorkerThread.h"
#include "Threading/JobSystem/JobSystem.h"
#include <random>

namespace ZE::JobSystem
{

/** Global condition_variable for sleeping workers */
std::condition_variable SleepConditionVariable;

std::condition_variable& CWorkerThread::GetSleepConditionVariable() { return SleepConditionVariable; }

CWorkerThread::CWorkerThread() : Active(false), Type(EWorkerThreadType::Full) {}

CWorkerThread::CWorkerThread(EWorkerThreadType InType,
	const std::thread::id& InThreadId) : Active(true), Type(InType), ThreadId(InThreadId) 
{

}

const SJob* CWorkerThread::TryGetOrStealJob(const size_t& InWorkerIdx)
{
	const SJob* Job = JobQueue.Pop();
	if(!Job && Type != EWorkerThreadType::Partial)
	{
		/** 
		 * Try to steal it from another worker
		 */
		std::uniform_int_distribution<size_t> Distribution(0, GetWorkerCount());

		size_t WorkerIdx = InWorkerIdx == -1 ? Distribution(std::random_device()) : InWorkerIdx;
		auto& WorkerToSteal = GetWorkerByIdx(WorkerIdx);
		
		/**
		 * Test if it isn't this worker
		 */
		if(*this != WorkerToSteal)
		{
			const SJob* Job = WorkerToSteal.JobQueue.Steal();
			if(Job)
				return Job;
		}

		std::this_thread::yield();
		return nullptr;
	}

	return Job;
}

void CWorkerThread::Flush()
{
	/** Try steal from the main worker, if no job found try again */
	const SJob* Job = TryGetOrStealJob(GetMainWorkerIdx());
	if (Job)
	{
		Internal::ExecuteJob(*Job);
	}
	else
	{
		const SJob* Job = TryGetOrStealJob();
		if(Job)
		{
			Internal::ExecuteJob(*Job);
		}
		else
		{
			if (Type != EWorkerThreadType::Partial)
			{
				std::unique_lock<std::mutex> Lock(SleepMutex);
				SleepConditionVariable.wait(Lock);
			}
		}
	}
}

}