#pragma once

#include <AbstractPort.hpp>
#include <PortUtils.hpp>
#include <memory>
#include "Platform.hpp"

class SerialPort : public AbstractPort {
public:
    LIBEXP SerialPort(std::string comPort, PortUtils::Serial::BaudRate baudRate = PortUtils::Serial::DEFAULT_COM_RATE);
    LIBEXP SerialPort(std::string comPort, PortUtils::Serial::PortConfig config);
    LIBEXP ~SerialPort();

    LIBEXP void flush() override;
    LIBEXP int connect() override;
    LIBEXP std::string read() override;
    LIBEXP std::size_t write(void *data, std::size_t data_len) override;
    LIBEXP static std::vector<std::string> getAvailablePorts();

private: 
    std::string comPort;
    PortUtils::Serial::PortConfig config;
    class SerialPortImpl;
    const SerialPortImpl *pimpl() const { return m_pimpl.get(); }
    SerialPortImpl *pimpl() { return m_pimpl.get(); }
    std::unique_ptr<SerialPortImpl> m_pimpl;
};