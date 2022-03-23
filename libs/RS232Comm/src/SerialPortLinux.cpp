//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Serial Port Linux Implementation class.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#ifdef __linux__

#include <include/SerialPort.hpp>
#include <include/Logger.hpp>
#include <filesystem>

class SerialPort::SerialPortImpl
{
public:
    SerialPortImpl();
    ~SerialPortImpl();

    void flush();
    std::string read();
    std::size_t write(void *data, std::size_t data_len);
    std::vector<std::string> getAvailablePorts();
};

SerialPort::SerialPort(std::string com_port) : com_port(com_port), m_pimpl(std::make_unique<SerialPortImpl>())
{
}

SerialPort::~SerialPort()
{
}

void SerialPort::flush(){ 
    pimpl()->flush();
}

std::size_t SerialPort::write(void *data, std::size_t data_len)
{
    return pimpl()->write(data, data_len);
}

std::string SerialPort::readIfAvailable()
{
    return pimpl()->read();
}

SerialPort::SerialPortImpl::SerialPortImpl(){

}

SerialPort::SerialPortImpl::~SerialPortImpl(){

}

void SerialPort::SerialPortImpl::flush(){
    
}

std::size_t SerialPort::SerialPortImpl::write(void *data, std::size_t data_len)
{
    (void)data;
    (void)data_len;
    return 0;
}

std::string SerialPort::SerialPortImpl::read()
{
    return "";
}

//###################################################################################################
// Get available serial ports on the machine.
//###################################################################################################

std::vector<std::string> SerialPort::getAvailablePorts()
{
    std::string path = "/dev";
    std::vector<std::string> ports;
    for(const auto& entry : std::filesystem::directory_iterator(path)) {
        if(entry.path().string().find("ttyUSB") != std::string::npos || entry.path().string().find("ttyACM") != std::string::npos) {
            ports.push_back(entry.path());
            LINFO("%s", entry.path().string().c_str());
        }
    }
    return ports;
}

#endif // __linux__.
