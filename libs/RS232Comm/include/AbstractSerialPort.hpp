#pragma once
#include <Windows.h>
#include <vector>

typedef struct SerialPortConfig {
	unsigned nComRate;
	unsigned nComBits;
	COMMTIMEOUTS timeout;
} SerialPortConf;

constexpr auto DEFAULT_COM_RATE = 9600;

class AbstractSerialPort {
public:
	AbstractSerialPort(const AbstractSerialPort&) = delete;
	//virtual ~AbstractSerialPort();
	
	static std::string* utf16ToUTF8(const std::wstring& s) {
		const int size = ::WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, NULL, 0, 0, NULL);
		std::vector<char> buf(size);
		::WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, &buf[0], size, 0, NULL);
		return new std::string(&buf[0]);
	}

private:

protected:
	AbstractSerialPort() {};
	~AbstractSerialPort() {};
	virtual std::vector<std::wstring> getAvailablePorts() = 0;
	virtual unsigned long sendData(void* buf, unsigned long szBuf) = 0;
};