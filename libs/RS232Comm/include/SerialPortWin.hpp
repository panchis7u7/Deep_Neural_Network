#pragma once
#include <include/AbstractSerialPort.hpp>
// Windows corresponding code.

typedef struct WinSerialPortConfig: SerialPortConf {
	COMMTIMEOUTS timeout;
} WinSerialPortConf;

class SerialPortWin: public AbstractSerialPort {
public:
	SerialPortWin(const wchar_t* comPort);
	SerialPortWin(const wchar_t* comPort, WinSerialPortConf& serialConf);
	~SerialPortWin();

	void setupEvent();

	//Polymorphic functions.
	std::vector<std::wstring> getAvailablePorts() override;
	std::size_t sendData(void* buf, std::size_t buf_len) override;
	std::size_t rcvData(void* buf, std::size_t buf_len) override;
	void initPort() override;
	void purgePort() override;
	void createPortFile();
	int SetComParms();

private:
	HANDLE hCom;
	WinSerialPortConf serialConf;
	const wchar_t* comPort;
};