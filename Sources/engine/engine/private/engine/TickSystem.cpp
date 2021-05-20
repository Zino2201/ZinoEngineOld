#include "engine/TickSystem.h"
#include <algorithm>
#include "engine/Engine.h"
#include <queue>

namespace ze::ticksystem
{

robin_hood::unordered_map<TickFlagBits, std::vector<Tickable*>> tickables;
std::queue<Tickable*> tickable_queue;

Tickable::Tickable() : can_tick(true), enable_tick(true) 
{
	register_tickable(this);
}

Tickable::~Tickable()
{
	unregister_tickable(this);
}

void register_tickable(Tickable* in_tickable) 
{
	tickable_queue.push(in_tickable);
}

void unregister_tickable(Tickable* in_tickable)
{
	TickFlags flags = in_tickable->get_tick_flags();

	if (flags & TickFlagBits::Fixed)
	{
		auto& array = tickables[TickFlagBits::Fixed];
		array.erase(std::remove(array.begin(), array.end(), in_tickable), array.end());
	}

	if (flags & TickFlagBits::Variable)
	{
		auto& array = tickables[TickFlagBits::Variable];
		array.erase(std::remove(array.begin(), array.end(), in_tickable), array.end());
	}

	if (flags & TickFlagBits::Late)
	{
		auto& array = tickables[TickFlagBits::Late];
		array.erase(std::remove(array.begin(), array.end(), in_tickable), array.end());
	}
}

void tick(TickFlagBits in_flag_bit, const float in_delta_time)
{
	/**
	 * Process tickables that wait for registration
	 */
	while(!tickable_queue.empty())
	{
		Tickable* tickable = tickable_queue.front();
		tickable_queue.pop();
		if(!tickable->can_ever_tick())
			continue;
		
		TickFlags flags = tickable->get_tick_flags();
		if (flags & TickFlagBits::Fixed)
		{
			auto& array = tickables[TickFlagBits::Fixed];
			array.emplace_back(tickable);
		}

		if (flags & TickFlagBits::Variable)
		{
			auto& array = tickables[TickFlagBits::Variable];
			array.emplace_back(tickable);
		}

		if (flags & TickFlagBits::Late)
		{
			auto& array = tickables[TickFlagBits::Late];
			array.emplace_back(tickable);
		}
	}

	for(auto& tickable : tickables[in_flag_bit])
	{
		TickFlags flags = tickable->get_tick_flags();
		
		if (flags & TickFlagBits::Fixed)
		{
			tickable->fixed_tick(in_delta_time);
		}

		if (flags & TickFlagBits::Variable)
		{
			tickable->variable_tick(in_delta_time);
		}

		if (flags & TickFlagBits::Late)
		{
			tickable->late_tick(in_delta_time);
		}
	}
}

};