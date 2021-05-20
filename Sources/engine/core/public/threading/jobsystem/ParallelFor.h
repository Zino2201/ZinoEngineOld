#pragma once

#include "JobSystem.h"
#include <span>

namespace ze::jobsystem
{

/**
 * Splitter to use for ParellelFor
 */

template<typename T>
struct NoSplit
{
	CORE_API bool split(const size_t& data_size) { return false; }
};

/**
 * Split on a threshold (default is 4)
 */
template<typename T, size_t Threshold = 4>
struct CountSplitter
{
	size_t count;
	size_t max_threshold;

	CountSplitter() : count(1)
	{
		max_threshold = Threshold;
	}

	bool split(const size_t& data_size) 
	{ 
		return count++ < MaxThreshold;
	}
};

namespace detail
{

template<typename T, typename LambdaType, typename SplitterType>
struct ParallelForJobData
{
	using DataType = T;

	size_t offset;
	std::span<T> data;
	LambdaType lambda;
	SplitterType splitter;

	ParallelForJobData(const std::span<T> in_data,
		const size_t& in_offset,
		const LambdaType& in_lambda,
		const SplitterType& in_splitter) 
		: data(in_data), offset(in_offset), lambda(in_lambda), splitter(in_splitter) {}
};

/**
 * Job function of ParallelFor
 */
template<typename T, typename Lambda, typename Splitter>
void parellel_for_impl(const Job& job)
{
	ParallelForJobData<T, Lambda, Splitter>* data = 
		job.get_userdata<ParallelForJobData<T, Lambda, Splitter>>();
	if(data)
	{
		/** Should we split into a job ? */
		if(data->splitter.split(data->data.size()))
		{
			std::span<T> left = data->data.subspan(0, 
				data->data.size() / 2);
			std::span<T> Right = data->data.subspan(Left.size(), 
				data->data.size() - left.size());

			const Job& left_job = 
				create_child_job_with_userdata<ParallelForJobData<T, Lambda, Splitter>>(
				&parellel_for_impl<T, Lambda, Splitter>,
				job,
				left,
				data->offset,
				data->lambda,
				data->splitter);
			schedule(left_job);

			const Job& right_job = 
				create_child_job_with_userdata<ParallelForJobData<T, Lambda, Splitter>>(
				&parellel_for_impl<T, Lambda, Splitter>,
				job,
				right,
				data->offset + left.size(),
				data->lambda,
				data->splitter);
			schedule(right_job);
		}
		else
		{
			for(size_t i = 0; i < data->data.size(); ++i)
			{
				data->lambda(i + data->offset);
			}
		}
	}
}

}

/**
 * ParallelFor implementation without dependence
 */
template<typename T, typename Lambda, 
	typename Splitter = CountSplitter<T>>
const Job& parellel_for(const size_t& size, T* data, const Lambda& lambda, const bool& in_schedule = true)
{
	static_assert(
		sizeof(detail::ParallelForJobData<T, Lambda, Splitter>) < Job::userdata_size, 
		"Lambda too big !");

	const Job& job =
		create_job_with_userdata<detail::ParallelForJobData<T, Lambda,
			Splitter>>(
			JobType::Normal,
			&detail::parellel_for_impl<T, Lambda, Splitter>,
			std::span<T>(data, size),
			0,
			lambda,
			Splitter());
	if(in_schedule)
		schedule(job);
	return job;
}

/**
 * ParallelFor implementation with dependence
 */
template<typename T, typename Lambda, 
	typename Splitter = CountSplitter<T>>
const Job& parellel_for(const size_t& size, T* data, const Job& dependence, 
	const Lambda& lambda, const bool& in_schedule = true)
{
	static_assert(
		sizeof(detail::ParallelForJobData<T, Lambda, Splitter>) < Job::userdata_size, 
		"Lambda too big !");

	const Job& job =
		create_job_with_userdata<detail::ParallelForJobData<T, Lambda, Splitter>>(
			JobType::Normal,
			&detail::parellel_for_impl<T, Lambda, Splitter>,
			std::span<T>(data, size),
			0,
			lambda,
			Splitter());
	if(in_schedule)
		schedule(job, dependence);
	return job;
}

}