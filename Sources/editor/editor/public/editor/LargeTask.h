#pragma once

#include "EngineCore.h"

namespace ze::editor
{

/**
 * Scoped large task
 */
class LargeTask
{
public:
	LargeTask(const std::string& in_text, const uint32_t& in_amount_of_work = 1);
	~LargeTask();

	void work(const uint32_t& in_amount = 1);
};

}