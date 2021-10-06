#pragma once
#include <string>
#include <memory>

class SerialBuffer {
public:
	SerialBuffer();
	//SerialBuffer(const SerialBuffer& sb);
	virtual ~SerialBuffer();
	void AddData(char ch);
	void AddData(std::string& szData);
	void AddData(std::string& szData, int iLen);
	void AddData(char* strData, int iLen);
	std::string GetData() { return m_szInternalBuffer; }
	inline long GetSize() { return m_szInternalBuffer.size(); }
	inline bool IsEmpty() { return m_szInternalBuffer.size() == 0; }
	void Flush();

private:
	class SerialBufferImpl;
	std::unique_ptr<SerialBufferImpl> m_pimpl;

	std::string m_szInternalBuffer;
	bool m_abLockAlways;
	long m_iCurPos;
	long m_alBytesUnRead;
	//void Init();
protected:
};