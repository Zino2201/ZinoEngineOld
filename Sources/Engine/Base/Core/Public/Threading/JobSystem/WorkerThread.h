#pragma once

#include "EngineCore.h"
#include "NonCopyable.h"
#include "JobDeque.h"
#include <atomic>
#include <condition_variable>

namespace ze::jobsystem
{

struct Job;

/**
 * Type of the worker thread
 */
enum class WorkerThreadType
{
	/** A worker that is run inside a thread dedicated to run jobs */
	Full,

	/** A worker that is inside a thread 
	 * can run jobs but do other stuff (typically the main thread for example) 
	 * Partial threads never yield and doesn't steal jobs and should never be waited from */
	Partial
};

/**
 * A worker thread
 */
class CORE_API WorkerThread
{
public:
	WorkerThread();
	WorkerThread(WorkerThreadType in_type,
		const std::thread::id& thread_id);

	template<typename T>
	WorkerThread(WorkerThreadType in_type, const T& thread_func) :
		type(in_type), active(true), thread(thread_func),
		thread_id(thread.get_id()) { }

	/** Flush the thread queue */
	void flush();

	/** Stop the worker */
	void stop();

	static std::condition_variable& get_sleep_condition_var();

	ZE_FORCEINLINE const WorkerThreadType& get_type() const { return type; }
	ZE_FORCEINLINE std::thread& get_thread() { return thread; }
	ZE_FORCEINLINE const std::thread::id& get_thread_id() const { return thread_id; }
	ZE_FORCEINLINE JobDeque& get_job_queue() { return job_queue; }
	ZE_FORCEINLINE bool is_active() const { return active; }
	ZE_FORCEINLINE bool has_jobs() const { return !job_queue.is_empty(); }

	bool operator==(const WorkerThread& other) const
	{
		return thread_id == other.thread_id;
	}

	bool operator!=(const WorkerThread& other) const
	{
		return thread_id != other.thread_id;
	}
private:
	const Job* try_get_or_steal_job(const size_t& in_worker_idx = -1);
private:
	WorkerThreadType type;
	std::atomic_bool active;
	std::thread thread;
	std::thread::id thread_id;
	JobDeque job_queue;
	std::mutex sleep_mutex;
};

}