#include <include/SerialBuffer.hpp>
#include <assert.h>
#include <iostream>


#ifndef  _WIN32 || _WIN64
typedef void* HANDLE;
#else
#include <Windows.h>
#endif	// ! _WIN32 || _WIN64

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Serial buffer implementation class.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class SerialBuffer::SerialBufferImpl {
public:
	SerialBuffer* m_sb;
	std::string m_szInternalBuffer;
	bool m_abLockAlways;
	long m_iCurPos;
	long m_alBytesUnRead;

#ifdef _WIN32 || _WIN64
	CRITICAL_SECTION m_csLock;
#elif __linux__

#endif

	SerialBufferImpl(SerialBuffer* sb);
	~SerialBufferImpl();
	inline void lockBuffer() { EnterCriticalSection(&m_csLock); }
	inline void unLockBuffer() { LeaveCriticalSection(&m_csLock); }
	void ClearAndReset();
	std::string GetBufferIfAvailable();
	long Read_N(std::string& szData, long alCount, HANDLE& hEventToReset);
	bool Read_Upto(std::string& szData, char chTerm, long& alBytesRead, HANDLE& hEventToReset);
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// End serial buffer implementation class.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//########################################################################################
// Implementation class Implementation.
//########################################################################################

SerialBuffer::SerialBufferImpl::SerialBufferImpl(SerialBuffer* sb) : m_sb(sb) {
	InitializeCriticalSection(&this->m_csLock);
	m_abLockAlways = true;
	m_iCurPos = 0;
	m_alBytesUnRead = 0;
	m_szInternalBuffer.erase();
}

SerialBuffer::SerialBufferImpl::~SerialBufferImpl() {
	DeleteCriticalSection(&this->m_csLock);
}

void SerialBuffer::SerialBufferImpl::ClearAndReset() {
	this->m_szInternalBuffer.erase();
	this->m_alBytesUnRead = 0;
	this->m_iCurPos = 0;
}

long SerialBuffer::SerialBufferImpl::Read_N(std::string& szData, long alCount, HANDLE& hEventToReset) {
	assert(hEventToReset != INVALID_HANDLE_VALUE);
	lockBuffer();
	long alTempCount = min(alCount, m_alBytesUnRead);
	long actualSize = m_sb->getSize();
	szData.append(m_szInternalBuffer, m_iCurPos, alTempCount);

	m_iCurPos += alTempCount;
	m_alBytesUnRead -= alTempCount;
	if (m_alBytesUnRead == 0) {
		ClearAndReset();
		ResetEvent(hEventToReset);
	}
	unLockBuffer();
	return actualSize;
}

std::string SerialBuffer::SerialBufferImpl::GetBufferIfAvailable() {
	if (m_szInternalBuffer.size() > 0)
		return m_szInternalBuffer;
}

bool SerialBuffer::SerialBufferImpl::Read_Upto(std::string& szData, char chTerm, long& alBytesRead, HANDLE& hEventToReset) {
	lockBuffer();
	alBytesRead = 0;

	bool abFound = false;
	if (this->m_alBytesUnRead > 0) {
		int iActualSize = m_sb->getSize();
		int iIncrementPos = 0;
		for (int i = this->m_iCurPos; i < iActualSize; ++i)
		{
			//szData .append ( m_szInternalBuffer,i,1);
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
	unLockBuffer();
	return abFound;
}

//########################################################################################
// Base class Implementation.
//########################################################################################

SerialBuffer::SerialBuffer() : m_pimpl(std::make_unique<SerialBufferImpl>(this)) {}

SerialBuffer::~SerialBuffer() {}

void SerialBuffer::addData(char ch) {
	m_pimpl->m_szInternalBuffer += ch;
	m_pimpl->m_alBytesUnRead += 1;
}

void SerialBuffer::addData(std::string& szData, int iLen) {
	m_pimpl->m_szInternalBuffer.append(szData.c_str(), iLen);
	m_pimpl->m_alBytesUnRead += iLen;
}

void SerialBuffer::addData(char* strData, int iLen) {
	m_pimpl->m_szInternalBuffer.append(strData, iLen);
	m_pimpl->m_alBytesUnRead += iLen;
}

void SerialBuffer::addData(std::string& szData) {
	m_pimpl->m_szInternalBuffer += szData;
	m_pimpl->m_alBytesUnRead += szData.size();
}

void SerialBuffer::flush() {
	m_pimpl->lockBuffer();
	m_pimpl->m_szInternalBuffer.erase();
	m_pimpl->m_alBytesUnRead = 0;
	m_pimpl->m_iCurPos = 0;
	m_pimpl->unLockBuffer();
}

//std::string SerialBuffer::getData() { return m_pimpl->m_szInternalBuffer; }

inline long SerialBuffer::getSize() { return m_pimpl->m_szInternalBuffer.size(); }

inline bool SerialBuffer::isEmpty() { return m_pimpl->m_szInternalBuffer.empty(); }

std::string SerialBuffer::GetDataIfAvailable() { return m_pimpl->GetBufferIfAvailable(); }

void SerialBuffer::LockBuffer() { m_pimpl->lockBuffer(); }

void SerialBuffer::UnLockBuffer() { m_pimpl->unLockBuffer(); }

//########################################################################################
// End Base class Implementation.
//########################################################################################