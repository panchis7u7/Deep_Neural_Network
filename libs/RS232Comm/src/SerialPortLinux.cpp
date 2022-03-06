//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Serial Port Linux Implementation class.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#if defined(__linux__) || defined(__APPLE__)

#include <include/SerialPort.hpp>

class SerialPort::SerialPortImpl
{
public:
    SerialPortImpl();
    ~SerialPortImpl();

    long initPort();
    long purgePort();
    std::string readIfAvailable();
    std::size_t write(void *data, std::size_t data_len);
    std::vector<std::string> getAvailablePorts();
};

SerialPort::SerialPort(std::string com_port) : com_port(com_port), m_pimpl(std::make_unique<SerialPortImpl>())
{
}

SerialPort::~SerialPort()
{
}

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

SerialPort::SerialPortImpl::SerialPortImpl(){

};

SerialPort::SerialPortImpl::~SerialPortImpl(){

};

long SerialPort::SerialPortImpl::initPort()
{
    return 0;
}

long SerialPort::SerialPortImpl::purgePort()
{
    return 0;
}

std::string SerialPort::SerialPortImpl::readIfAvailable()
{
    return "";
}

std::size_t SerialPort::SerialPortImpl::write(void *data, std::size_t data_len)
{
    (void)data;
    (void)data_len;
    return 0;
}

std::vector<std::string> SerialPort::SerialPortImpl::getAvailablePorts()
{
    return {};
}

#endif // __linux__.