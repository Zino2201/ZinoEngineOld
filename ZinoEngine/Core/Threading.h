#pragma once

#include "EngineCore.h"

/**
 * A simple semaphore that can be resetted
 */
class CSemaphore
{
public:
    CSemaphore()
        : bNotified(false) {}

    void Notify()
    {
        if(!bIsWaiting)
            return;

        bNotified = true;
        Condition.notify_one();
    }

    void Wait()
    {
        std::unique_lock<std::mutex> Lock(Mutex);
        bIsWaiting = true;
        while(!bNotified)
        {
            Condition.wait(Lock);
        }

        bNotified = false;
        bIsWaiting = false;
    }

    bool HasWaiter() const { return bIsWaiting; }
private:
    std::mutex Mutex;
    std::condition_variable Condition;
    bool bNotified;
    std::atomic_bool bIsWaiting;
};