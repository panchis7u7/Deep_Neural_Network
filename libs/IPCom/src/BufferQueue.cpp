#include <include/BufferQueue.hpp>
#include <include/LockGuard.hpp>
#include <include/SharedMessage.hpp>
#include <include/Logger.hpp>
#include <assert.h>

BufferQueue::BufferQueue(unsigned queueLen, std::string& shmem_name, std::string& err_message): 
    m_uQueueLength(queueLen), 
    m_qShared(new(shmem_name, err_message) SharedBufferQueue(queueLen)) {}

bool BufferQueue::try_read(DataGlob& d){
    Cell* cellPtr;
    
    {
        // make sure index operation is locked.
        // Index operation is light-weighted.
        // Using a lock doesn't hurt performance much.
        LockGuard lock(m_qShared->getOperationLock());

        if (m_iRead_idx >= m_qShared->getWriteIdx()) return false; 

        // Jump to lastest message.
        // It is the flexibility of shared memory. In TCP, you can't do this.
        if (m_iRead_idx < m_qShared->getFarthestReadIdx()) {
            const int32_t last_read_idx = m_iRead_idx;
            m_iRead_idx = m_qShared->getFarthestReadIdx().load();
            // It doesn't hold when int overflow
            assert(last_read_idx < m_iRead_idx);
        }

        cellPtr = &m_qShared->getQueueSharedMessages()[m_iRead_idx % m_uQueueLength];

        // Using a semaphore to track how many process is reading the current message.
        // "signal" a reader is here
        cellPtr->m_sSem.increase();

        // Check if someone is writing to this cell
        // This only happen if the queue warp around.
        if (!cellPtr->m_lWriterLock.tryLock()) {
            cellPtr->m_sSem.decrease();
            LINFO("try_read|someone is writing");
            LINFO("try_read|m.test_num :%f", cellPtr->m_dgData.m_dCheckSum);
            return false;
        } else {
            // Unlock it since I lock the cell in if statement.
            cellPtr->m_lWriterLock.unLock();
        }

        ++m_iRead_idx;

    }

    d = cellPtr->m_dgData;
    // TODO: what if program crashes here? RAII ?
    cellPtr->m_sSem.decrease();
    return true;
}

bool BufferQueue::read(DataGlob& d) { return try_read(d); }
bool BufferQueue::write(const DataGlob& d) { 
    m_qShared->write(d); 
    return true;
}
