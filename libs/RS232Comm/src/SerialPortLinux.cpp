//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Serial Port Linux Implementation class.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#ifdef __linux__

#include <include/SerialPort.hpp>

class SerialPort::SerialPortImpl
{
public:
    SerialPortImpl(const std::string com_port) {};
    ~SerialPortImpl();

    long initPort();
    long purgePort();
    std::string readIfAvailable();
    std::size_t write(void *data, std::size_t data_len);
    std::vector<std::string> getAvailablePorts();
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

#endif // __linux__.