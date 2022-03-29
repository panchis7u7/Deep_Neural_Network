#pragma once

#include <include/SpinLock.hpp>
using Lock = SpinLock;

LIBEXP class LockGuard {
public:
    LockGuard(SpinLock& lock): m_lLock(lock) { m_lLock.lock(); } 
    ~LockGuard() { m_lLock.unLock(); }
private:
    Lock& m_lLock;
};
