#ifdef __linux__
#include <include/SerialPortLinux.hpp>
#include <iostream>

SerialPortLinux::SerialPortLinux(const std::string com_port)
{
    // TODO: Constructor.
    std::cout << com_port << std::endl;
}

SerialPortLinux::~SerialPortLinux()
{
    // TODO: Destructor.
}

long SerialPortLinux::initPort()
{
    // TODO: Port Initialization.
    return 0;
}

long SerialPortLinux::purgePort()
{
    // TODO: Port cleanup.
    return 0;
}

std::string SerialPortLinux::readIfAvailable()
{
    // TODO: Port read.
    return "";
}

std::size_t SerialPortLinux::write(void *data, std::size_t data_len)
{
    std::cout << data << data_len << std::endl;
    // TODO: Port write.
    return 0;
}

std::vector<std::string> SerialPortLinux::getAvailablePorts()
{
    // TODO: Get available ports in machine.
    return {};
}

#endif // __linux__