#pragma once

#include <include/AbstractPort.hpp>
#include <memory>
#include <../platform.hpp>

class SerialPort : public AbstractPort
{
public:
    LIBEXP SerialPort(std::string com_port);
    LIBEXP ~SerialPort();

    LIBEXP void flush() override;
    LIBEXP std::string read() override;
    LIBEXP std::size_t write(void *data, std::size_t data_len) override;
    LIBEXP static std::vector<std::string> getAvailablePorts();

private: 
    std::string com_port;
    class SerialPortImpl;
    const SerialPortImpl *pimpl() const { return m_pimpl.get(); }
    SerialPortImpl *pimpl() { return m_pimpl.get(); }
    std::unique_ptr<SerialPortImpl> m_pimpl;
};