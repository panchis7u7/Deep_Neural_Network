#pragma once

#include "../platform.hpp"
#include <include/SharedMessage.hpp>
#include <include/SpinLock.hpp>
#include <memory>
#include <atomic>

using Lock = SpinLock;

LIBEXP class SharedBufferQueue {
public:
    SharedBufferQueue(unsigned QueueLength);
    ~SharedBufferQueue();
    bool try_write(DataGlob& dataGlob);
    bool write(DataGlob& dataGlob);
private:
    unsigned m_uQueueLength;
    // Circular Buffer begin read and write indexes.
    std::atomic<int32_t> m_atiWrite_Idx{ 0 };
    std::atomic<int32_t> m_atiFarthest_Read_Idx{ 0 };
    Lock m_lOperationLock;
    Cell* m_ceQueueSharedMessages;

    // class SharedBufferImpl;
    // std::unique_ptr<SharedBufferImpl> m_pimpl;
    // const SharedBufferImpl* pimpl() const { return m_pimpl.get(); }
    // SharedBufferImpl* pimpl() { return m_pimpl.get(); }
};