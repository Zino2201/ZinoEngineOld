#pragma once

#include "EngineCore.h"
#include "NonCopyable.h"
#include "JobDeque.h"
#include <atomic>
#include <condition_variable>

namespace ZE::JobSystem
{

struct SJob;

/**
 * Type of the worker thread
 */
enum class EWorkerThreadType
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
class ENGINECORE_API CWorkerThread
{
public:
	CWorkerThread();
	CWorkerThread(EWorkerThreadType InType,
		const std::thread::id& InThreadId);

	template<typename T>
	CWorkerThread(EWorkerThreadType InType, const T& InThreadFunc) :
		Active(true), Type(InType), Thread(InThreadFunc), 
		ThreadId(Thread.get_id()) { }

	/** Flush the thread queue */
	void Flush();

	static std::condition_variable& GetSleepConditionVariable();

	ZE_FORCEINLINE const EWorkerThreadType& GetType() const { return Type; }
	ZE_FORCEINLINE std::thread& GetThread() { return Thread; }
	ZE_FORCEINLINE const std::thread::id& GetThreadId() const { return ThreadId; }
	ZE_FORCEINLINE TJobDeque& GetJobQueue() { return JobQueue; }
	ZE_FORCEINLINE bool IsActive() const { return Active; }
	ZE_FORCEINLINE bool HasJobs() const { return !JobQueue.IsEmpty(); }

	bool operator==(const CWorkerThread& InOther) const
	{
		return ThreadId == InOther.ThreadId;
	}

	bool operator!=(const CWorkerThread& InOther) const
	{
		return ThreadId != InOther.ThreadId;
	}
private:
	const SJob* TryGetOrStealJob(const size_t& InWorkerIdx = -1);
private:
	EWorkerThreadType Type;
	std::atomic_bool Active;
	std::thread Thread;
	std::thread::id ThreadId;
	TJobDeque JobQueue;
	std::mutex SleepMutex;
};

}