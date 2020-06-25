#pragma once

#include "EngineCore.h"
#include "NonCopyable.h"
#include "JobDeque.h"
#include <atomic>

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
	 * can run jobs but do other stuff (typically the main thread for example) */
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

	FORCEINLINE const std::thread::id& GetThreadId() const { return ThreadId; }
	FORCEINLINE TJobDeque& GetJobQueue() { return JobQueue; }
	FORCEINLINE bool IsActive() const { return Active; }
	FORCEINLINE bool HasJobs() const { return !JobQueue.IsEmpty(); }

	bool operator==(const CWorkerThread& InOther) const
	{
		return ThreadId == InOther.ThreadId;
	}

	bool operator!=(const CWorkerThread& InOther) const
	{
		return ThreadId != InOther.ThreadId;
	}
private:
	const SJob* TryGetOrStealJob();
private:
	EWorkerThreadType Type;
	std::atomic_bool Active;
	std::thread Thread;
	std::thread::id ThreadId;
	TJobDeque JobQueue;
};



}