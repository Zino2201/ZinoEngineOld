#include "threading/jobsystem/Job.h"
#include "threading/jobsystem/WorkerThread.h"
#include "threading/jobsystem/JobSystem.h"

namespace ze::jobsystem
{

void schedule(const Job& job)
{
	if(job.type != JobType::Lightweight)
	{
		get_worker().get_job_queue().push(&job);	

		/** Wake one random worker */
		WorkerThread::get_sleep_condition_var().notify_one();
	}
	else
		detail::execute(job);
}

void schedule(const Job& job, const Job& dependance)
{
	ZE_ASSERT(dependance.dependent_count + 1 < Job::max_dependents);

	job.dependances_count++;
	dependance.dependents[dependance.dependent_count++] = &job;
}

void wait(const Job& job)
{
	while (!job.is_finished())
	{
		WorkerThread::get_sleep_condition_var().notify_one();
		get_worker().flush();
	}
}

void detail::execute(const Job& job)
{
	job.function(job);
	finish(job);
}

void detail::finish(const Job& job)
{
	job.unfinished_jobs--;

	if (job.is_finished())
	{
		/** Tell the parent that we finished */
		if (job.parent)
		{
			finish(*job.parent);
		}

		/** Run dependents */
		for (uint8_t i = 0; i < job.dependent_count; ++i)
		{
			const Job* dependent = job.dependents[i];
			if(dependent)
			{
				switch(dependent->type)
				{
				default:
					get_worker().get_job_queue().push(dependent);
					break;
				case JobType::Lightweight:
					execute(*dependent);
					break;
				}
			}
		}
	}
}

}