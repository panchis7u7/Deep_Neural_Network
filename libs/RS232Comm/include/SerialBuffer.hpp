#pragma once
#include <string>
#include <memory>

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Serial buffer class.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class SerialBuffer {
public:
	SerialBuffer();
	virtual ~SerialBuffer();
	void addData(char ch);
	void addData(std::string& szData);
	void addData(std::string& szData, int iLen);
	void addData(char* strData, int iLen);
	std::string GetDataIfAvailable();
	void flush();
	void LockBuffer();
	void UnLockBuffer();

	inline long getSize();
	inline bool isEmpty();

private:
	class SerialBufferImpl;
	std::unique_ptr<SerialBufferImpl> m_pimpl;
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// End Serial buffer class.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@