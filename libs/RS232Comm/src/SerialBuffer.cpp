#include <include/SerialBuffer.hpp>
#include <assert.h>

#ifndef  _WIN32 || _WIN64
typedef void* HANDLE;
#else
#include <Windows.h>
#endif	// ! _WIN32 || _WIN64

class SerialBuffer::SerialBufferImpl {
public:
	std::string m_szInternalBuffer;
	bool m_abLockAlways;
	long m_iCurPos;
	long m_alBytesUnRead;
#ifdef _WIN32 || _WIN64
	CRITICAL_SECTION m_csLock;
#endif
	void ClearAndReset(HANDLE& hEventToReset);
	SerialBuffer* m_sb;

	SerialBufferImpl(SerialBuffer* sb): m_sb(sb) {
		InitializeCriticalSection(&this->m_csLock);
		m_abLockAlways = true;
		m_iCurPos = 0;
		m_alBytesUnRead = 0;
		m_szInternalBuffer.erase();
	};

	virtual ~SerialBufferImpl() {
		DeleteCriticalSection(&this->m_csLock);
	}

	inline void lockBuffer() { EnterCriticalSection(&m_csLock); }
	inline void unLockBuffer() { LeaveCriticalSection(&m_csLock); }

	long Read_N(std::string& szData, long alCount, HANDLE& hEventToReset) {
		assert(hEventToReset != INVALID_HANDLE_VALUE);
		lockBuffer();
		long alTempCount = min(alCount, m_alBytesUnRead);
		long actualSize = m_sb->getSize();
		szData.append(m_szInternalBuffer, m_iCurPos, alTempCount);

		m_iCurPos += alTempCount;
		m_alBytesUnRead -= alTempCount;
		if (m_alBytesUnRead == 0) {
			ClearAndReset(hEventToReset);
		}
		unLockBuffer();
	}

	bool Read_Available(std::string& szData, HANDLE& hEventToReset) {
		lockBuffer();
		szData += m_szInternalBuffer;
		ClearAndReset(hEventToReset);
		unLockBuffer();
		return(szData.size() > 0);
	}

	bool Read_Upto(std::string& szData, char chTerm, long& alBytesRead, HANDLE& hEventToReset) {
		return Read_Upto_FIX(szData, chTerm, alBytesRead, hEventToReset);

		lockBuffer();
	}
	
	bool Read_Upto_FIX(std::string& szData, char chTerm, long& alBytesRead, HANDLE& hEventToReset) {
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
				ClearAndReset(hEventToReset);
			}
		}
		unLockBuffer();
		return abFound;
	}
};

void SerialBuffer::SerialBufferImpl::ClearAndReset(HANDLE& hEventToReset) {
	this->m_szInternalBuffer.erase();
	this->m_alBytesUnRead = 0;
	this->m_iCurPos = 0;
	ResetEvent(hEventToReset);
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

std::string SerialBuffer::getData() { return m_pimpl->m_szInternalBuffer; }

inline long SerialBuffer::getSize() { return m_pimpl->m_szInternalBuffer.size(); }

inline bool SerialBuffer::isEmpty() { return m_pimpl->m_szInternalBuffer.empty(); }

void SerialBuffer::lockBuffer() { m_pimpl->lockBuffer(); }

void SerialBuffer::unLockBuffer() { m_pimpl->unLockBuffer(); }

//########################################################################################
// End Base class Implementation.
//########################################################################################