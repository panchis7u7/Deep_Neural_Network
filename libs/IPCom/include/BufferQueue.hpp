#include <include/SharedMessage.hpp>

LIBEXP class BufferQueue {
public:
    bool try_read(DataGlob& d);
    bool read(DataGlob& d);
    bool write(const DataGlob& d);
private:
     // The read idx for this reader.
    int32_t m_iRead_idx{ 0 };

    // No ownership, just a plain old pointer.
    // User is responsible to assign the pointer.
    MutipleReaderQueueShared* m_qShared;
};