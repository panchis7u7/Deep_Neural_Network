//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Serial Port Linux Implementation class.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#ifdef __linux__

#include <include/SerialPort.hpp>
#include <include/Logger.hpp>
#include <filesystem>
#include <sys/types.h>
#include <signal.h>

class SerialPort::SerialPortImpl
{
public:
    SerialPortImpl(SerialPort* base);
    ~SerialPortImpl();

    void clean();
    void clean(pid_t process);
    void flush();
    int connect();
    std::string read();
    std::size_t write(void *data, std::size_t data_len);
    std::vector<std::string> getAvailablePorts();
private:
    SerialPort* m_spBase;
    static constexpr int SFD_UNAVAILABLE = -1;
    int m_iFd = SFD_UNAVAILABLE;
};

//###################################################################################################
// Platform independent abstraction definitions.
//###################################################################################################

SerialPort::SerialPort(std::string com_port) : com_port(com_port), m_pimpl(std::make_unique<SerialPortImpl>(this)) {}
SerialPort::~SerialPort() {}
int SerialPort::connect() { return pimpl()->connect(); }
void SerialPort::flush(){ pimpl()->flush(); }
std::size_t SerialPort::write(void *data, std::size_t data_len) { return pimpl()->write(data, data_len); }
std::string SerialPort::read() { return pimpl()->read(); }

//###################################################################################################
// Linux platform implementation.
//###################################################################################################

void SerialPort::SerialPortImpl::clean(){
    if(m_iFd != SFD_UNAVAILABLE)
        close(m_iFd);
}

void SerialPort::SerialPortImpl::clean(pid_t process){
    kill(process, SIGKILL);
}

//###################################################################################################
// Serial Port Initzialization. (Constructor)
//###################################################################################################

SerialPort::SerialPortImpl::SerialPortImpl(SerialPort* base) { this->m_spBase = base; }

//###################################################################################################
// Serial Port Cleanup. (Destructor)
//###################################################################################################

SerialPort::SerialPortImpl::~SerialPortImpl(){ clean(); }

//###################################################################################################
// Clean serial buffer.
//###################################################################################################

void SerialPort::SerialPortImpl::flush(){
    
}

//###################################################################################################
// Open a serial port.
//###################################################################################################

int SerialPort::SerialPortImpl::connect() {

    return 0;
}

//###################################################################################################
// Read serial buffer data if available.
//###################################################################################################

std::string SerialPort::SerialPortImpl::read()
{
    return "";
}

//###################################################################################################
// Write to serial buffer if available.
//###################################################################################################

std::size_t SerialPort::SerialPortImpl::write(void *data, std::size_t data_len)
{
    (void)data;
    (void)data_len;
    return 0;
}

//###################################################################################################
// Get available serial ports on the machine.
//###################################################################################################

std::vector<std::string> SerialPort::getAvailablePorts()
{
    std::string path = "/dev";
    std::vector<std::string> ports;
    for(const auto& entry : std::filesystem::directory_iterator(path)) {
        if(entry.path().string().find("USB") != std::string::npos) {
            ports.push_back(entry.path());
            LINFO("%s", entry.path().string().c_str());
        }
    }
    return ports;
}


#endif // __linux__.
