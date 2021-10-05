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

	static unsigned int __stdcall eventThreadFn(void* pvParam);

	//Polymorphic functions.
	std::vector<std::wstring> getAvailablePorts() override;
	std::size_t sendData(void* buf, std::size_t buf_len) override;
	std::size_t rcvData(void* buf, std::size_t buf_len) override;
	HRESULT initPort() override;
	HRESULT purgePort() override;
	HRESULT createPortFile();
	HRESULT setComParms();
	HRESULT setupEvent();

private:
	SERIAL_STATE m_eState;
	HANDLE m_hCom;
	HANDLE m_hThreadTerm;
	HANDLE m_hThread;
	HANDLE	m_hThreadStarted;
	HANDLE	m_hDataRx;
	bool m_abIsConnected;
	WinSerialPortConf m_wSerialConf;
	const wchar_t* m_wComPort;
};