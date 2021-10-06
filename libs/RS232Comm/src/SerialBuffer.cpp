#include <include/SerialBuffer.hpp>
#include <assert.h>

#ifdef _WIN32 || _WIN64
#include <Windows.h>

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
		this->LockBuffer();
		//long alTempCount = min(alCount, m_alBytesUnRead);
	}

	bool Read_Upto(std::string& szData, char chTerm, long& alBytesRead, HANDLE& hEventToReset);
	bool Read_Available(std::string& szData, HANDLE& hEventToReset);
	bool Read_Upto_FIX(std::string& szData, char chTerm, long& alBytesRead, HANDLE& hEventToReset);

private:
	CRITICAL_SECTION m_csLock;
	void ClearAndReset(HANDLE& hEventToReset);
	SerialBuffer* m_sb;
};

SerialBuffer::SerialBuffer(): m_pimpl(new SerialBuffer::SerialBufferImpl(this)) {
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

#endif // _WIN32 || _WIN64