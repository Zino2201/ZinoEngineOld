#pragma once

#include "EngineCore.h"
#include <atomic>
#include <array>

namespace ZE::JobSystem
{

static constexpr size_t GJobAlignement = std::hardware_destructive_interference_size;

enum class EJobType
{
	/** 
	 * The default job type,
	 * Jobs of this type will always be queue to a worker thread to not block any execution
	 */
	Normal,

	/**
	 * Mark this job has lightweight
	 * Allow this job to be directly executed
	 * If the job has any dependencies, it will also be directly executed instead to be enqueued
	 */
	Lightweight,
};

/**
 * Structure that represents a job
 * You can create jobs using ZE::JobSystem::CreateJob, ZE::JobSystem::CreateChildJob,
 *	ZE::JobSystem::CreateJobUserdata
 * To schedule a job, use ZE::JobSystem::ScheduleJob
 * You can wait for a job using ZE::JobSystem::WaitJob
 */
struct alignas(GJobAlignement) ENGINECORE_API SJob
{
	using JobFunction = void(*)(const SJob& InJob);
	static constexpr size_t UserdataSize = 64;
	static constexpr size_t MaxDependents = 16;
	static constexpr uint8_t MaxChilds = 255;

	EJobType Type;
	const char* Name;
	const SJob* Parent;
	/** Unfinished job count, 0 = finished, 1 = not finished, > 1 = childs running */
	mutable std::atomic_uint8_t UnfinishedJobs;
	const JobFunction Function;
	mutable std::atomic_uint8_t DependancesCount;
	mutable std::atomic_uint8_t DependentCount;
	mutable std::array<const SJob*, MaxDependents> Dependents;
	mutable std::array<uint8_t, UserdataSize> Userdata;

	SJob() : Type(EJobType::Normal), Parent(nullptr), UnfinishedJobs(0), Function(nullptr) {}
	SJob(const JobFunction& InJobFunction, EJobType InType = EJobType::Normal,
		const SJob* InParent = nullptr) : 
		Type(InType),
		Function(InJobFunction), Parent(InParent), UnfinishedJobs(1), DependancesCount(0),
		DependentCount(0) {}
		 
	/**
	 * Get casted user data
	 */
	template<typename T>
	FORCEINLINE T* GetUserdata() const
	{
		return reinterpret_cast<T*>(Userdata.data());
	}

	FORCEINLINE bool IsFinished() const { return UnfinishedJobs == 0; }
};

/** Schedule the job */
ENGINECORE_API void ScheduleJob(const SJob& InJob);

/** Schedule the job after the specified job finish */
ENGINECORE_API void ScheduleJob(const SJob& InJob, const SJob& InDependence);

/** Wait for a job */
ENGINECORE_API void WaitJob(const SJob& InJob);

namespace Internal
{
	/**
	 * Execute the job
	 */
	ENGINECORE_API void ExecuteJob(const SJob& InJob);

	/**
	 * Finish the job
	 * If this job has any childs, it will wait
	 * If this job has dependents, it will execute them
	 */
	ENGINECORE_API void FinishJob(const SJob& InJob);
}


}