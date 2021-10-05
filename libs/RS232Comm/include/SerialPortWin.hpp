#pragma once
#include <include/AbstractSerialPort.hpp>
// Windows corresponding code.

class SerialPortWin: AbstractSerialPort {
public:
	SerialPortWin(const wchar_t* comPort);
	SerialPortWin(const wchar_t* comPort, SerialPortConf& serialConf);
	~SerialPortWin();
	void initPort(int nComRate, int nComBits, COMMTIMEOUTS timeout);
	void purgePort();
	void outputToPort(LPCVOID buf, DWORD szBuf);
	unsigned long sendData(void* buf, unsigned long szBuf) override;
	DWORD inputFromPort(LPVOID buf, DWORD szBuf);
	std::vector<std::wstring> getAvailablePorts();

	friend const char* operator<<(SerialPortWin& serialPort, const char* text);

	//Sub-Functions
	void createPortFile();
	int SetComParms(int nComRate, int nComBits, COMMTIMEOUTS timeout);

private:
	HANDLE hCom;
	const wchar_t* comPort;
};