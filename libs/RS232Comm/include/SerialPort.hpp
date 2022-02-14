#pragma once

#include <include/AbstractPort.hpp>
#include <memory>

/* #ifdef _WIN32
#include <include/SerialPortWin.hpp>
typedef SerialPortWin SerialPort;
#else
#include <include/SerialPortLinux.hpp>
typedef SerialPortLinux SerialPort;
#endif //_WIN32 */

class SerialPort : public AbstractPort
{
public:
    SerialPort(std::string com_port);
    ~SerialPort();

    long initPort() override;
    long purgePort() override;
    std::string readIfAvailable() override;
    std::size_t write(void *data, std::size_t data_len) override;
    std::vector<std::string> getAvailablePorts() override;

private:
    std::string com_port;
    class SerialPortImpl;
    const SerialPortImpl *pimpl() const { return m_pimpl.get(); }
    SerialPortImpl *pimpl() { return m_pimpl.get(); }
    std::unique_ptr<SerialPortImpl> m_pimpl;
};