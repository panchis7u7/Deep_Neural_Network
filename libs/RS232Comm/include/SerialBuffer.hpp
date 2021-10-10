#pragma once
#include <string>
#include <memory>

class SerialBuffer {
public:
	SerialBuffer();
	virtual ~SerialBuffer();
	void addData(char ch);
	void addData(std::string& szData);
	void addData(std::string& szData, int iLen);
	void addData(char* strData, int iLen);
	std::string getData();
	inline long getSize();
	inline bool isEmpty();
	void lockBuffer();
	void unLockBuffer();
	void flush();

private:
	class SerialBufferImpl;
	std::unique_ptr<SerialBufferImpl> m_pimpl;
};