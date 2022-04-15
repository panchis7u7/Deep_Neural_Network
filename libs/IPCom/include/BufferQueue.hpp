#include <include/SharedMessage.hpp>
#include <include/SharedBufferQueue.hpp>

LIBEXP class BufferQueue {
public:
    BufferQueue(unsigned queueLen, std::string& shmem_name, std::string& err_message);
    bool try_read(DataGlob& d);
    bool read(DataGlob& d);
    bool write(const DataGlob& d);
private:
     // The read idx for this reader.
    int32_t m_iRead_idx{ 0 };
    unsigned m_uQueueLength;

    // No ownership, just a plain old pointer.
    // User is responsible to assign the pointer.
    SharedBufferQueue* m_qShared;
};