#pragma once

#include "JobSystem.h"
#include <span>

namespace ZE
{

/**
 * Splitter to use for ParellelFor
 */

template<typename T>
struct TNoSplit
{
	FORCEINLINE bool Split(const size_t& InDataSize) { return false; }
};

/**
 * Split on a threshold (default is worker count)
 */
template<typename T, size_t Threshold = -1>
struct TCountSplitter
{
	size_t Count;
	size_t MaxThreshold;

	TCountSplitter() : Count(1)
	{
		if(Threshold == -1)
			MaxThreshold = ZE::JobSystem::GetWorkerCount();
		else 
			MaxThreshold = Threshold;
	}

	bool Split(const size_t& InDataSize) 
	{ 
		return Count++ < MaxThreshold;
	}
};

namespace ParallelForInternal
{

template<typename T, typename LambdaType, typename SplitterType>
struct SParallelForJobData
{
	using DataType = T;

	size_t Offset;
	std::span<T> Data;
	LambdaType Lambda;
	SplitterType Splitter;

	SParallelForJobData(const std::span<T> InData,
		const size_t& InOffset,
		const LambdaType& InLambda,
		const SplitterType& InSplitter) 
		: Data(InData), Offset(InOffset), Lambda(InLambda), Splitter(InSplitter) {}
};

/**
 * Job function of ParallelFor
 */
template<typename T, typename Lambda, typename Splitter>
void ParellelForJobFunc(const ZE::JobSystem::SJob& InJob)
{
	SParallelForJobData<T, Lambda, Splitter>* Data = 
		InJob.GetUserdata<SParallelForJobData<T, Lambda, Splitter>>();
	if(Data)
	{
		/** Should we split into a job ? */
		if(Data->Splitter.Split(Data->Data.size()))
		{
			std::span<T> Left = Data->Data.subspan(0, 
				Data->Data.size() / 2);
			std::span<T> Right = Data->Data.subspan(Left.size(), 
				Data->Data.size() - Left.size());

			const ZE::JobSystem::SJob& LeftJob = 
				ZE::JobSystem::CreateChildJobUserdata<SParallelForJobData<T, Lambda, Splitter>>(
				&ParellelForJobFunc<T, Lambda, Splitter>,
				InJob,
				Left,
				Data->Offset,
				Data->Lambda,
				Data->Splitter);
			ZE::JobSystem::ScheduleJob(LeftJob);

			const ZE::JobSystem::SJob& RightJob = 
				ZE::JobSystem::CreateChildJobUserdata<SParallelForJobData<T, Lambda, Splitter>>(
				&ParellelForJobFunc<T, Lambda, Splitter>,
				InJob,
				Right,
				Data->Offset + Left.size(),
				Data->Lambda,
				Data->Splitter);

			ZE::JobSystem::ScheduleJob(RightJob);
		}
		else
		{
			for(size_t i = 0; i < Data->Data.size(); ++i)
			{
				Data->Lambda(i + Data->Offset);
			}
		}
	}
}

}

/**
 * ParallelFor implementation without dependence
 */
template<typename T, typename Lambda, bool bSchedule = true, 
	typename Splitter = TCountSplitter<T>>
const ZE::JobSystem::SJob& ParallelFor(const size_t& InSize, T* InData, const Lambda& InLambda)
{
	static_assert(
		sizeof(ParallelForInternal::SParallelForJobData<T, Lambda, Splitter>) < ZE::JobSystem::SJob::UserdataSize, 
		"Lambda too big !");

	const ZE::JobSystem::SJob& Job =
		ZE::JobSystem::CreateJobUserdata<ParallelForInternal::SParallelForJobData<T, Lambda,
			Splitter>>(
			EJobType::Normal,
			&ParallelForInternal::ParellelForJobFunc<T, Lambda, Splitter>,
			std::span<T>(InData, InSize),
			0,
			InLambda,
			Splitter());
	if constexpr(bSchedule)
		ZE::JobSystem::ScheduleJob(Job);
	return Job;
}

/**
 * ParallelFor implementation with dependence
 */
template<typename T, typename Lambda, bool bSchedule = true,
	typename Splitter = TCountSplitter<T>>
	const ZE::JobSystem::SJob& ParallelFor(const size_t& InSize, T* InData, 
		const ZE::JobSystem::SJob& InDependence, const Lambda& InLambda)
{
	static_assert(
		sizeof(ParallelForInternal::SParallelForJobData<T, Lambda, Splitter>) < ZE::JobSystem::SJob::UserdataSize,
		"Lambda too big !");

	const ZE::JobSystem::SJob& Job =
		ZE::JobSystem::CreateJobUserdata<ParallelForInternal::SParallelForJobData<T, Lambda,
		Splitter>>(
			&ParallelForInternal::ParellelForJobFunc<T, Lambda, Splitter>,
			std::span<T>(InData, InSize),
			0,
			InLambda,
			Splitter());
	if constexpr (bSchedule)
		ZE::JobSystem::ScheduleJob(Job, InDependence);
	return Job;
}

/** Parallel for specializations (no dependence) */
template<typename T, typename Lambda, bool bSchedule = true, 
	typename Splitter = TCountSplitter<T>>
const ZE::JobSystem::SJob& ParallelFor(std::span<T>& InData, const Lambda& InLambda)
{
	return ParallelFor<T, Lambda, bSchedule, Splitter>(InData.size(), InData.data(), InLambda);
}

template<typename T, size_t Size, 
	typename Lambda, bool bSchedule = true, 
	typename Splitter = TCountSplitter<T>>
const ZE::JobSystem::SJob& ParallelFor(std::array<T, Size>& InData, const Lambda& InLambda)
{
	return ParallelFor<T, Lambda, bSchedule, Splitter>(InData.size(), InData.data(), InLambda);
}

template<typename T, typename Lambda, bool bSchedule = true,
	typename Splitter = TCountSplitter<T>>
	const ZE::JobSystem::SJob& ParallelFor(std::vector<T>& InData, const Lambda& InLambda)
{
	return ParallelFor<T, Lambda, bSchedule, Splitter>(InData.size(), InData.data(), InLambda);
}

/** ParrallelFor specialization with dependances */
template<typename T, typename Lambda, bool bSchedule = true,
	typename Splitter = TCountSplitter<T>>
	const ZE::JobSystem::SJob& ParallelFor(std::span<T>& InData, 
		const ZE::JobSystem::SJob& InDependence,
		const Lambda& InLambda)
{
	return ParallelFor<T, Lambda, bSchedule, Splitter>(InData.size(), InData.data(), InDependence,
		InLambda);
}

template<typename T, size_t Size,
	typename Lambda, bool bSchedule = true,
	typename Splitter = TCountSplitter<T>>
	const ZE::JobSystem::SJob& ParallelFor(std::array<T, Size>& InData, 
		const ZE::JobSystem::SJob& InDependence,
		const Lambda& InLambda)
{
	return ParallelFor<T, Lambda, bSchedule, Splitter>(InData.size(), InData.data(), InDependence,
		InLambda);
}

template<typename T, typename Lambda, bool bSchedule = true,
	typename Splitter = TCountSplitter<T>>
	const ZE::JobSystem::SJob& ParallelFor(std::vector<T>& InData, 
		const ZE::JobSystem::SJob& InDependence,
		const Lambda& InLambda)
{
	return ParallelFor<T, Lambda, bSchedule, Splitter>(InData.size(), InData.data(), InDependence,
		InLambda);
}

}