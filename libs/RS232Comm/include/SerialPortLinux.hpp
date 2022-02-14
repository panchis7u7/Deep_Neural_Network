#pragma once

#ifdef __linux__
#include <include/AbstractPort.hpp>

class SerialPortLinux : public AbstractPort
{

public:
    SerialPortLinux(const std::string com_port);
    ~SerialPortLinux();

    long initPort() override;
    long purgePort() override;
    std::string readIfAvailable() override;
    std::size_t write(void *data, std::size_t data_len) override;
    std::vector<std::string> getAvailablePorts() override;
};
#endif //__linux__