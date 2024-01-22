#include <include/SharedBufferQueue.hpp>
#include <include/SharedMessage.hpp>
#include <include/LockGuard.hpp>
#include "spdlog/spdlog.h"
#include <assert.h>

SharedBufferQueue::SharedBufferQueue(unsigned QueueLength): m_uQueueLength(QueueLength){
    m_ceQueueSharedMessages = new Cell[QueueLength];
}

SharedBufferQueue::~SharedBufferQueue(){
    delete[] m_ceQueueSharedMessages;
}

bool SharedBufferQueue::try_write(const DataBlob& DataBlob){
    Cell& m = m_ceQueueSharedMessages[m_atiWrite_idx % m_uQueueLength];

    // If some processes is reading, do nothing
    if (!m.m_sSem.isZero()) return false;

    // The write operation is protected by a lock *in the message*.
    {
        LockGuard lock(m.m_lWriterLock);
        m.m_dgData = DataBlob;

        {
            // Index operation is protected by lock for *write* and *read*.
            // Because index operation is light,
            // using a lock doesn't hurt the performance much.
            LockGuard lock(m_lOperationLock);
            ++m_atiWrite_idx;

            // Queue warp around.
            if ((m_atiWrite_idx - m_atiFarthest_Read_idx) > (int)m_uQueueLength) {
                int32_t last_farest_read_idx = m_atiFarthest_Read_idx;
                m_atiFarthest_Read_idx++;
                assert(m_atiFarthest_Read_idx = m_atiWrite_idx - m_uQueueLength);
                // doesn't hold when int overflow
                assert(m_atiFarthest_Read_idx > last_farest_read_idx);
            }
        }
    }
    return true;
}

bool SharedBufferQueue::write(const DataBlob& DataBlob){
    while (!try_write(DataBlob)) {
        // Unlikely. Happends when the queue warp around.
        spdlog::debug("write|spinning");
        usleep(1000);
    }
    return true;
}
