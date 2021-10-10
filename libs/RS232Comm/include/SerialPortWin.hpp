#pragma once
#include <include/AbstractPort.hpp>
#include <include/SerialBuffer.hpp>

typedef struct WinSerialPortConfig: SerialPortConf {
	COMMTIMEOUTS timeout;
} WinSerialPortConf;

class SerialPortWin: public AbstractPort {
public:
	SerialPortWin(const wchar_t* comPort);
	SerialPortWin(const wchar_t* comPort, WinSerialPortConf& serialConf);
	~SerialPortWin();

	static unsigned int __stdcall eventThreadFn(void* pvParam);

	HRESULT createPortFile();
	HRESULT setComParms();
	HRESULT setupEvent();
	void invalidateHandle(HANDLE& hHandle);
	void closeAndCleanHandle(HANDLE& hHandle);
	SERIAL_STATE GetCurrentState() { return m_eState; }

	//Polymorphic functions.
	HRESULT initPort() override;
	HRESULT purgePort() override;
	std::vector<std::wstring> getAvailablePorts() override;
	std::size_t write(void* data, std::size_t data_len) override;
	std::size_t read(void* buf, std::size_t buf_len) override;

private:
	HANDLE m_hCom;
	HANDLE m_hThreadTerm;
	HANDLE m_hThread;
	HANDLE m_hThreadStarted;
	HANDLE m_hDataRx;
	SERIAL_STATE m_eState;
	SerialBuffer m_serialBuffer;
	WinSerialPortConf m_wSerialConf;
	bool m_abIsConnected;
	const wchar_t* m_wComPort;
};