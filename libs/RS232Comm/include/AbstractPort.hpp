#pragma once
#include <Windows.h>
#include <vector>

class AbstractPort {
public:
	AbstractPort(const AbstractPort&) = delete;
	//~AbstractPort();

	//###################################################################################################
	// Polymorphic operations.
	//###################################################################################################

	virtual long InitPort() = 0;
	virtual long PurgePort() = 0;
	virtual std::string ReadIfAvailable() = 0;
	virtual std::size_t Write(void* data, std::size_t data_len) = 0;
	virtual std::vector<std::wstring> GetAvailablePorts() = 0;

	//###################################################################################################
	// Operator Overloading.
	//###################################################################################################

	friend const char* operator<<(AbstractPort& serialPort, const char* text);
	friend void operator>>(AbstractPort& serialPort, std::string& str);

protected:
	AbstractPort() {};
	~AbstractPort() {};
};

const char* operator<<(AbstractPort& serialPort, const char* text) {
	serialPort.Write((void*)text, strlen(text));
	return text;
}

void operator>>(AbstractPort& serialPort, std::string& str) {
	str += serialPort.ReadIfAvailable();
}