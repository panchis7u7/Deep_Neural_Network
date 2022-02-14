//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Serial Port Windows Implementation class.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#ifdef _WIN64

#include <include/AbstractPort.hpp>
#include <include/SerialBuffer.hpp>
#include <include/PortUtils.hpp>
#include <include/SerialPort.hpp>

using namespace PortUtils::Serial;

typedef struct WinSerialPortConfig : SerialPortConfig
{
    COMMTIMEOUTS timeout;
} WinSerialPortConf;

class SerialPort::SerialPortImpl
{
    SerialPortWin(const wchar_t *comPort);
    SerialPortWin(const wchar_t *comPort, WinSerialPortConf &serialConf);
    ~SerialPortWin();

    static unsigned int __stdcall eventThreadFn(void *pvParam);

    HRESULT CreatePortFile();
    HRESULT SetComParms();
    HRESULT SetupEvent();
    HRESULT CanProcess();

    void InvalidateHandle(HANDLE &hHandle);
    void CloseAndCleanHandle(HANDLE &hHandle);
    inline void SetDataReadEvent() { SetEvent(m_hDataRx); }
    inline SerialState GetCurrentState() { return m_eState; }
    std::size_t Read(void *buf, std::size_t buf_len);

    // Polymorphic functions.
    HRESULT InitPort() override;
    HRESULT PurgePort() override;
    std::size_t Write(void *data, std::size_t data_len) override;
    std::string ReadIfAvailable() override;
    std::vector<std::wstring> GetAvailablePorts() override;

private:
    bool m_aIsReading;
    bool m_abIsConnected;
    HANDLE m_hCom;
    HANDLE m_hThreadTerm;
    HANDLE m_hThread;
    HANDLE m_hThreadStarted;
    HANDLE m_hDataRx;
    SerialState m_eState;
    SerialBuffer m_serialBuffer;
    WinSerialPortConf m_wSerialConf;
    const wchar_t *m_wComPort;
};

SerialPort::SerialPort(std::string com_port) : m_pimpl(std::make_unique<SerialPortImpl>(com_port)), com_port(com_port) {}

long SerialPort::initPort()
{
    return pimpl()->initPort();
}

long SerialPort::purgePort()
{
    return pimpl()->purgePort();
}

std::size_t SerialPort::write(void *data, std::size_t data_len)
{
    return pimpl()->write(data, data_len);
}

std::string SerialPort::readIfAvailable()
{
    return pimpl()->readIfAvailable();
}

std::vector<std::string> SerialPort::getAvailablePorts()
{
    return pimpl()->getAvailablePorts();
}

#endif // _WIN64.