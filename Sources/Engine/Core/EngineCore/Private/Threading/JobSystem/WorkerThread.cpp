#include "Threading/JobSystem/WorkerThread.h"
#include "Threading/JobSystem/JobSystem.h"
#include <random>

namespace ZE::JobSystem
{

/** CWorkerThread */
CWorkerThread::CWorkerThread() : Active(false), Type(EWorkerThreadType::Full) {}

CWorkerThread::CWorkerThread(EWorkerThreadType InType,
	const std::thread::id& InThreadId) : Active(true), Type(InType), ThreadId(InThreadId) 
{

}

const SJob* CWorkerThread::TryGetOrStealJob()
{
	const SJob* Job = JobQueue.Pop();
	if(!Job && Type != EWorkerThreadType::Partial)
	{
		/** 
		 * Try to steal it from another worker
		 */
		std::uniform_int_distribution<size_t> Distribution(0, GetWorkerCount());

		size_t WorkerIdx = Distribution(std::random_device());
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
	const SJob* Job = TryGetOrStealJob();
	if (Job)
	{
		Internal::ExecuteJob(*Job);
	}
}

}