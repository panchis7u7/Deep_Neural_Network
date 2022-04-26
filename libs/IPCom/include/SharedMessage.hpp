#pragma once

#include <include/SpinLock.hpp>
#include <include/Semaphore.hpp>
#include "../platform.hpp"

using Lock = SpinLock;

constexpr size_t RAW_DATA_CHAR_SIZE = 1024 * 1024 * 10;

LIBEXP struct DataBlob {
    double m_dCheckSum = -1;
    char m_carrRawData[RAW_DATA_CHAR_SIZE];
};

LIBEXP struct Cell {
    Lock m_lWriterLock;
    Semaphore m_sSem;
    DataBlob m_dgData;
};