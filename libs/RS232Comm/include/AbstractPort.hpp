#pragma once
#include <string>
#include <string.h>
#include <vector>

class AbstractPort
{
public:
	AbstractPort(const AbstractPort &) = delete;
	//~AbstractPort();

	//###################################################################################################
	// Polymorphic operations.
	//###################################################################################################

	virtual long initPort() = 0;
	virtual long purgePort() = 0;
	virtual std::string readIfAvailable() = 0;
	virtual std::size_t write(void *data, std::size_t data_len) = 0;
	virtual std::vector<std::string> getAvailablePorts() = 0;

	//###################################################################################################
	// Operator Overloading.
	//###################################################################################################

	friend const char *operator<<(AbstractPort &port, const char *text);
	friend void operator>>(AbstractPort &port, std::string &str);

protected:
	AbstractPort(){};
	~AbstractPort(){};
};

const char *operator<<(AbstractPort &port, const char *text)
{
	port.write((void *)text, strlen(text));
	return text;
}

void operator>>(AbstractPort &port, std::string &str)
{
	str += port.readIfAvailable();
}