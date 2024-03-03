#pragma once

#include <SpinLock.hpp>
#include <Semaphore.hpp>
#include "Platform.hpp"

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