#pragma once

#include "../platform.hpp"
#include <atomic>

LIBEXP class Semaphore {
public:
    inline void increase() { m_iCount++; }
    inline void decrease() { m_iCount--; }
    inline bool isZero(){
        int32_t expect = 0;
        return m_iCount.compare_exchange_strong(expect, 0);
    }
private:
    std::atomic<int> m_iCount;
};
