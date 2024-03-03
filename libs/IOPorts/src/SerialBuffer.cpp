#include <SerialBuffer.hpp>
#include <assert.h>
#include <iostream>
//https://stackoverflow.com/questions/54392470/how-to-handle-buffering-serial-data
#ifndef _WIN32
typedef void *HANDLE;
#else
#include <Windows.h>
#include <condition_variable>

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Serial buffer implementation class.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class SerialBuffer::SerialBufferImpl
{
public:
	SerialBuffer *m_sb;
	std::string m_szInternalBuffer;
	long m_iCurPos;
	long m_alBytesUnRead;
	bool m_abLockAlways;
	bool m_aIsAvailable = true;
	bool m_aWrittenToBuffer = false;

private:
	std::mutex m_mxBuffer;
	std::condition_variable m_cvBuffer;

public:
#ifdef _WIN32 || _WIN64
	CRITICAL_SECTION m_csLock;
#elif __linux__

#endif

	SerialBufferImpl(SerialBuffer *sb);
	~SerialBufferImpl();
	void ClearAndReset();
	inline void LockBuffer();
	inline void UnLockBuffer();
	std::string GetBufferIfAvailable();
	long Read_N(std::string &szData, long alCount, HANDLE &hEventToReset);
	bool Read_Upto(std::string &szData, char chTerm, long &alBytesRead, HANDLE &hEventToReset);
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// End serial buffer implementation class.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//########################################################################################
// Implementation class Implementation.
//########################################################################################

SerialBuffer::SerialBufferImpl::SerialBufferImpl(SerialBuffer *sb) : m_sb(sb)
{
	InitializeCriticalSection(&this->m_csLock);
	m_abLockAlways = true;
	m_iCurPos = 0;
	m_alBytesUnRead = 0;
	m_szInternalBuffer.erase();
}

SerialBuffer::SerialBufferImpl::~SerialBufferImpl()
{
	DeleteCriticalSection(&this->m_csLock);
}

void SerialBuffer::SerialBufferImpl::ClearAndReset()
{
	this->m_szInternalBuffer.erase();
	this->m_alBytesUnRead = 0;
	this->m_iCurPos = 0;
}

inline void SerialBuffer::SerialBufferImpl::LockBuffer()
{
	m_aIsAvailable = false;
	EnterCriticalSection(&m_csLock);
}

inline void SerialBuffer::SerialBufferImpl::UnLockBuffer()
{
	LeaveCriticalSection(&m_csLock);
	m_aIsAvailable = true;
	m_cvBuffer.notify_one();
}

long SerialBuffer::SerialBufferImpl::Read_N(std::string &szData, long alCount, HANDLE &hEventToReset)
{
	assert(hEventToReset != INVALID_HANDLE_VALUE);
	LockBuffer();
	long alTempCount = min(alCount, m_alBytesUnRead);
	long actualSize = m_sb->getSize();
	szData.append(m_szInternalBuffer, m_iCurPos, alTempCount);

	m_iCurPos += alTempCount;
	m_alBytesUnRead -= alTempCount;
	if (m_alBytesUnRead == 0)
	{
		ClearAndReset();
		ResetEvent(hEventToReset);
	}
	UnLockBuffer();
	return actualSize;
}

std::string SerialBuffer::SerialBufferImpl::GetBufferIfAvailable()
{
	std::unique_lock<std::mutex> lock(m_mxBuffer);
	m_cvBuffer.wait(lock, [&]()
					{ return (m_aIsAvailable && m_aWrittenToBuffer) ? true : false; });
	m_aWrittenToBuffer = false;
	return m_szInternalBuffer;
}

bool SerialBuffer::SerialBufferImpl::Read_Upto(std::string &szData, char chTerm, long &alBytesRead, HANDLE &hEventToReset)
{
	LockBuffer();
	alBytesRead = 0;

	bool abFound = false;
	if (this->m_alBytesUnRead > 0)
	{
		int iActualSize = m_sb->getSize();
		int iIncrementPos = 0;
		for (int i = this->m_iCurPos; i < iActualSize; ++i)
		{
			// szData .append ( m_szInternalBuffer,i,1);
			szData += this->m_szInternalBuffer[i];
			this->m_alBytesUnRead -= 1;
			if (this->m_szInternalBuffer[i] == chTerm)
			{
				iIncrementPos++;
				abFound = true;
				break;
			}
			iIncrementPos++;
		}
		this->m_iCurPos += iIncrementPos;
		if (this->m_alBytesUnRead == 0)
		{
			ClearAndReset();
			ResetEvent(hEventToReset);
		}
	}
	UnLockBuffer();
	return abFound;
}

//########################################################################################
// Base class Implementation.
//########################################################################################

SerialBuffer::SerialBuffer() : m_pimpl(std::make_unique<SerialBufferImpl>(this)) {}

SerialBuffer::~SerialBuffer() {}

void SerialBuffer::addData(char ch)
{
	m_pimpl->m_szInternalBuffer += ch;
	m_pimpl->m_alBytesUnRead += 1;
	m_pimpl->m_aWrittenToBuffer = true;
}

void SerialBuffer::addData(std::string &szData, int iLen)
{
	m_pimpl->m_szInternalBuffer.append(szData.c_str(), iLen);
	m_pimpl->m_alBytesUnRead += iLen;
	m_pimpl->m_aWrittenToBuffer = true;
}

void SerialBuffer::addData(char *strData, int iLen)
{
	m_pimpl->m_szInternalBuffer.append(strData, iLen);
	m_pimpl->m_alBytesUnRead += iLen;
	m_pimpl->m_aWrittenToBuffer = true;
}

void SerialBuffer::addData(std::string &szData)
{
	m_pimpl->m_szInternalBuffer += szData;
	m_pimpl->m_alBytesUnRead += szData.size();
	m_pimpl->m_aWrittenToBuffer = true;
}

void SerialBuffer::flush()
{
	m_pimpl->LockBuffer();
	m_pimpl->m_szInternalBuffer.erase();
	m_pimpl->m_alBytesUnRead = 0;
	m_pimpl->m_iCurPos = 0;
	m_pimpl->UnLockBuffer();
}

// std::string SerialBuffer::getData() { return m_pimpl->m_szInternalBuffer; }

inline long SerialBuffer::getSize() { return m_pimpl->m_szInternalBuffer.size(); }

inline bool SerialBuffer::isEmpty() { return m_pimpl->m_szInternalBuffer.empty(); }

std::string SerialBuffer::GetDataIfAvailable() { return m_pimpl->GetBufferIfAvailable(); }

void SerialBuffer::LockBuffer() { m_pimpl->LockBuffer(); }

void SerialBuffer::UnLockBuffer() { m_pimpl->UnLockBuffer(); }

//########################################################################################
// End Base class Implementation.
//########################################################################################

#endif // ! _WIN32 || _WIN64