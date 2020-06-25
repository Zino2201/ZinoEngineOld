#include "Threading/JobSystem/Job.h"
#include "Threading/JobSystem/WorkerThread.h"
#include "Threading/JobSystem/JobSystem.h"

namespace ZE::JobSystem
{

void ScheduleJob(const SJob& InJob)
{
	if(InJob.Type != EJobType::Lightweight)
		GetWorker().GetJobQueue().Push(&InJob);
	else
		Internal::ExecuteJob(InJob);
}

void ScheduleJob(const SJob& InJob, const SJob& InDependence)
{
	must(InDependence.DependentCount + 1 < SJob::MaxDependents);

	InJob.DependancesCount++;
	InDependence.Dependents[InDependence.DependentCount++] = &InJob;
}

void WaitJob(const SJob& InJob)
{
	while (!InJob.IsFinished())
	{
		GetWorker().Flush();
	}
}

void Internal::ExecuteJob(const SJob& InJob)
{
	InJob.Function(InJob);
	FinishJob(InJob);
}

void Internal::FinishJob(const SJob& InJob)
{
	InJob.UnfinishedJobs--;

	if (InJob.IsFinished())
	{
		/** Tell the parent that we finished */
		if (InJob.Parent)
		{
			FinishJob(*InJob.Parent);
		}

		/** Run dependents */
		for (uint8_t i = 0; i < InJob.DependentCount; ++i)
		{
			const SJob* Dependent = InJob.Dependents[i];
			if(Dependent)
			{
				switch(Dependent->Type)
				{
				default:
					GetWorker().GetJobQueue().Push(Dependent);
					break;
				case EJobType::Lightweight:
					ExecuteJob(*Dependent);
					break;
				}
			}
		}
	}
}

}