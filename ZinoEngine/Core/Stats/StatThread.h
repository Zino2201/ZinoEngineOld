#pragma once

#include "Core/Engine.h"

/**
 * Stat thread class
 */
class CStatThread
{
public:
    static void Start();
    static void Stop();
private:
    static void Main();
private:
    static std::thread Handle;
    static std::atomic_bool Loop;
};
