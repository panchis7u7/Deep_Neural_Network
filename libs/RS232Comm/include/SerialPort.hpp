#pragma once

#include <include/AbstractPort.hpp>
#include <memory>

class SerialPort : public AbstractPort
{
public:
    SerialPort(std::string com_port);
    ~SerialPort();

    void flush() override;
    std::string readIfAvailable() override;
    std::size_t write(void *data, std::size_t data_len) override;
    static std::vector<std::string> getAvailablePorts();

private: 
    std::string com_port;
    class SerialPortImpl;
    const SerialPortImpl *pimpl() const { return m_pimpl.get(); }
    SerialPortImpl *pimpl() { return m_pimpl.get(); }
    std::unique_ptr<SerialPortImpl> m_pimpl;
};