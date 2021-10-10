#pragma once
#include <Windows.h>
#include <vector>

typedef struct SerialPortConfig {
	unsigned nComRate;
	unsigned nComBits;
	unsigned byStopBits;
	unsigned parity;
} SerialPortConf;

typedef enum tagSERIAL_STATE {
	SS_Unknown,
	SS_UnInit,
	SS_Init,
	SS_Started,
	SS_Stopped,
} SERIAL_STATE;

constexpr auto DEFAULT_COM_RATE = 9600;

class AbstractPort {
public:
	AbstractPort(const AbstractPort&) = delete;
	//virtual ~AbstractSerialPort();
	
	static std::string* utf16ToUTF8(const std::wstring& s) {
		const int size = ::WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, NULL, 0, 0, NULL);
		std::vector<char> buf(size);
		::WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, &buf[0], size, 0, NULL);
		return new std::string(&buf[0]);
	}

	friend const char* operator<<(AbstractPort& serialPort, const char* text) {
		serialPort.write(text, strlen(text));
		return text;
	}

	/*friend void operator>>(AbstractSerialPort& serialPort, std::string& str) {
		return str;
	}*/

private:

protected:
	AbstractPort() {};
	~AbstractPort() {};
	virtual std::vector<std::wstring> getAvailablePorts() = 0;
	virtual std::size_t write(const char* data, std::size_t data_len) = 0;
	virtual std::size_t rcvData(void* buf, std::size_t buf_len) = 0;
	virtual long initPort() = 0;
	virtual long purgePort() = 0;
};