#pragma once
#include <include/AbstractPort.hpp>
#include <include/SerialBuffer.hpp>
#include <include/PortUtils.hpp>

using namespace PortUtils::Serial;

typedef struct WinSerialPortConfig: SerialPortConfig {
	COMMTIMEOUTS timeout;
} WinSerialPortConf;

class SerialPortWin: public AbstractPort {
public:
	SerialPortWin(const wchar_t* comPort);
	SerialPortWin(const wchar_t* comPort, WinSerialPortConf& serialConf);
	~SerialPortWin();

	static unsigned int __stdcall eventThreadFn(void* pvParam);

	HRESULT CreatePortFile();
	HRESULT SetComParms();
	HRESULT SetupEvent();
	HRESULT CanProcess();
	//HRESULT	ReadAvailable(std::string& data);
	void InvalidateHandle(HANDLE& hHandle);
	void CloseAndCleanHandle(HANDLE& hHandle);
	PortUtils::Serial::SerialState GetCurrentState() { return m_eState; }
	inline void SetDataReadEvent() { SetEvent(m_hDataRx); }
	std::size_t Read(void* buf, std::size_t buf_len);

	//Polymorphic functions.
	HRESULT InitPort() override;
	HRESULT PurgePort() override;
	std::size_t Write(void* data, std::size_t data_len) override;
	void Read(std::string& buf) override;
	std::vector<std::wstring> GetAvailablePorts() override;

private:
	HANDLE m_hCom;
	HANDLE m_hThreadTerm;
	HANDLE m_hThread;
	HANDLE m_hThreadStarted;
	HANDLE m_hDataRx;
	SerialState m_eState;
	SerialBuffer m_serialBuffer;
	WinSerialPortConf m_wSerialConf;
	bool m_abIsConnected;
	const wchar_t* m_wComPort;
};