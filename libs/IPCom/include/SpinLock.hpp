#pragma once

#include "../platform.hpp"
#include <thread>
#include <atomic>
#include <unistd.h>

// A spinlock is a lock that causes a thread trying to acquire it 
// to simply wait in a loop while repeatedly checking whether the 
// lock is available.

LIBEXP class SpinLock {
    public:
        inline bool tryLock() { return !m_afFlag.test_and_set(); };
        void lock(){ while(!tryLock()) usleep(m_uSpeedTimeUs); };
        void unLock(){ m_afFlag.clear(); };
    private:
        // std::atomic_flag is guaranteed to be lock-free.
        std::atomic_flag m_afFlag { ATOMIC_FLAG_INIT };
        uint32_t m_uSpeedTimeUs = 50;
};
