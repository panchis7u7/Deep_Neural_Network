#pragma once

#include <memory>
#include <atomic>
#include <include/SharedAlloc.hpp>
#include <include/SharedMessage.hpp>
#include <include/SpinLock.hpp>
#include "../platform.hpp"

using Lock = SpinLock;

LIBEXP class SharedBufferQueue {
public:
    SharedBufferQueue(unsigned QueueLength);
    ~SharedBufferQueue();
    bool try_write(const DataBlob& DataBlob);
    bool write(const DataBlob& DataBlob);

    Lock& getOperationLock() { return m_lOperationLock; }
    std::atomic<int32_t>& getWriteIdx() { return m_atiWrite_idx; }
    std::atomic<int32_t>& getFarthestReadIdx() { return m_atiFarthest_Read_idx; }
    Cell* getQueueSharedMessages() { return m_ceQueueSharedMessages; }

    void* operator new(size_t size, std::string& shmem_name, std::string& err_message) {
        return (void*)shalloc<SharedBufferQueue>(shmem_name, err_message);
    }

private:
    unsigned m_uQueueLength;
    // Circular Buffer begin read and write indexes.
    std::atomic<int32_t> m_atiWrite_idx{ 0 };
    std::atomic<int32_t> m_atiFarthest_Read_idx{ 0 };
    Lock m_lOperationLock;
    Cell* m_ceQueueSharedMessages;

    // class SharedBufferImpl;
    // std::unique_ptr<SharedBufferImpl> m_pimpl;
    // const SharedBufferImpl* pimpl() const { return m_pimpl.get(); }
    // SharedBufferImpl* pimpl() { return m_pimpl.get(); }
};