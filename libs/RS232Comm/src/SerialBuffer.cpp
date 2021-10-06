#include <include/SerialBuffer.hpp>
#include <assert.h>

#ifndef  _WIN32 || _WIN64
typedef void* HANDLE;
#else
#include <Windows.h>
#endif	// ! _WIN32 || _WIN64

class SerialBuffer::SerialBufferImpl {
public:
	SerialBufferImpl(SerialBuffer* sb): m_sb(sb) {
		InitializeCriticalSection(&this->m_csLock); 
	};

	virtual ~SerialBufferImpl() {
		DeleteCriticalSection(&this->m_csLock);
	}

	inline void LockBuffer() { EnterCriticalSection(&m_csLock); }
	inline void UnLockBuffer() { LeaveCriticalSection(&m_csLock); }

	long Read_N(std::string& szData, long alCount, HANDLE& hEventToReset) {
		assert(hEventToReset != INVALID_HANDLE_VALUE);
		LockBuffer();
		long alTempCount = min(alCount, m_sb->m_alBytesUnRead);
		long actualSize = m_sb->GetSize();
		szData.append(m_sb->m_szInternalBuffer, m_sb->m_iCurPos, alTempCount);

		m_sb->m_iCurPos += alTempCount;
		m_sb->m_alBytesUnRead -= alTempCount;
		if (m_sb->m_alBytesUnRead == 0) {
			ClearAndReset(hEventToReset);
		}
		UnLockBuffer();
	}

	bool Read_Available(std::string& szData, HANDLE& hEventToReset) {
		LockBuffer();
		szData += m_sb->m_szInternalBuffer;
		ClearAndReset(hEventToReset);
		UnLockBuffer();
		return(szData.size() > 0);
	}

	bool Read_Upto(std::string& szData, char chTerm, long& alBytesRead, HANDLE& hEventToReset) {
		return Read_Upto_FIX(szData, chTerm, alBytesRead, hEventToReset);

		LockBuffer();
	}
	
	bool Read_Upto_FIX(std::string& szData, char chTerm, long& alBytesRead, HANDLE& hEventToReset) {
		LockBuffer();
		alBytesRead = 0;

		bool abFound = false;
		if (this->m_sb->m_alBytesUnRead > 0) {
			int iActualSize = this->m_sb->GetSize();
			int iIncrementPos = 0;
			for (int i = this->m_sb->m_iCurPos; i < iActualSize; ++i)
			{
				//szData .append ( m_szInternalBuffer,i,1);
				szData += this->m_sb->m_szInternalBuffer[i];
				this->m_sb->m_alBytesUnRead -= 1;
				if (this->m_sb->m_szInternalBuffer[i] == chTerm)
				{
					iIncrementPos++;
					abFound = true;
					break;
				}
				iIncrementPos++;
			}
			this->m_sb->m_iCurPos += iIncrementPos;
			if (this->m_sb->m_alBytesUnRead == 0)
			{
				ClearAndReset(hEventToReset);
			}
		}
		UnLockBuffer();
		return abFound;
	}

private:
#ifdef _WIN32 || _WIN64
	CRITICAL_SECTION m_csLock;
#endif
	void ClearAndReset(HANDLE& hEventToReset);
	SerialBuffer* m_sb;
};

void SerialBuffer::SerialBufferImpl::ClearAndReset(HANDLE& hEventToReset) {
	this->m_sb->m_szInternalBuffer.erase();
	this->m_sb->m_alBytesUnRead = 0;
	this->m_sb->m_iCurPos = 0;
	ResetEvent(hEventToReset);
}

SerialBuffer::SerialBuffer() : m_pimpl(new SerialBuffer::SerialBufferImpl(this)) {
	this->m_abLockAlways = true;
	this->m_iCurPos = 0;
	this->m_alBytesUnRead = 0;
	this->m_szInternalBuffer.erase();
}

/*SerialBuffer::SerialBuffer(const SerialBuffer& sb) :
	m_szInternalBuffer(sb.m_szInternalBuffer),
	m_abLockAlways(sb.m_abLockAlways),
	m_iCurPos(sb.m_iCurPos),
	m_alBytesUnRead(sb.m_alBytesUnRead)
{}*/

SerialBuffer::~SerialBuffer() {}

void SerialBuffer::AddData(char ch) {
	this->m_szInternalBuffer += ch;
	this->m_alBytesUnRead += 1;
}

void SerialBuffer::AddData(std::string& szData, int iLen) {
	this->m_szInternalBuffer.append(szData.c_str(), iLen);
	this->m_alBytesUnRead += iLen;
}

void SerialBuffer::AddData(char* strData, int iLen) {
	this->m_szInternalBuffer.append(strData, iLen);
	this->m_alBytesUnRead += iLen;
}

void SerialBuffer::AddData(std::string& szData) {
	this->m_szInternalBuffer += szData;
	this->m_alBytesUnRead += szData.size();
}

void SerialBuffer::Flush() {
	this->m_pimpl->LockBuffer();
	this->m_szInternalBuffer.erase();
	this->m_alBytesUnRead = 0;
	this->m_iCurPos = 0;
	this->m_pimpl->UnLockBuffer();
}