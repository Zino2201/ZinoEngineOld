#pragma once

#include "EngineCore.h"

class CActor;

/**
 * World state
 */
struct SWorldState
{
	std::vector<std::shared_ptr<CActor>> Actors;
};

/**
 * Game state
 * Used for synchronize data between threads, contains data about the game
 */
struct SGameState
{
	SWorldState WorldState;
};
