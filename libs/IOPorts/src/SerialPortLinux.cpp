//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Serial Port Linux Implementation class.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "Platform.hpp"

#ifdef LINUX_PLATFORM

#include <filesystem>
#include <cstring>
#include <SerialPort.hpp>
#include <SharedAlloc.hpp>
#include <BufferQueue.hpp>
#include <SharedMessage.hpp>
#include <PortUtils.hpp>
// Serial configuration.
#include <signal.h>
#include <termios.h>
#include <unistd.h> // write(), read(), close(), fork()
#include <fcntl.h>  // Contains file controls like O_RDWR
#include <errno.h>  // Error integer and strerror() function
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/mman.h>

class SerialPort::SerialPortImpl
{
public:
    SerialPortImpl(SerialPort* base, PortUtils::Serial::BaudRate baudRate = PortUtils::Serial::DEFAULT_COM_RATE);
    SerialPortImpl(SerialPort* base, PortUtils::Serial::PortConfig config);
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
    pid_t* m_pidChildProcess;
    static constexpr int SFD_UNAVAILABLE = -1;
    static constexpr unsigned MAX_EVENTS = 64;
    int m_iFd = SFD_UNAVAILABLE;
    int m_iEpollFd = SFD_UNAVAILABLE;
};

//###################################################################################################
// Platform independent abstraction definitions.
//###################################################################################################

SerialPort::SerialPort(std::string comPort, PortUtils::Serial::BaudRate baudRate) : comPort(comPort), m_pimpl(std::make_unique<SerialPortImpl>(this, baudRate)) {
    std::string err = "";
    std::string shmemName = PortUtils::shMemPortNameParser(comPort, "/");

    m_bqBuffer = new BufferQueue(12, shmemName, err);
    if (err != "") spdlog::error(err.c_str());
    config.nComRate = baudRate;
}

SerialPort::SerialPort(std::string comPort, PortUtils::Serial::PortConfig config): comPort(comPort), m_pimpl(std::make_unique<SerialPortImpl>(this, config)) {
    std::string err = "";
    std::string shmemName = PortUtils::shMemPortNameParser(comPort, "/");

    m_bqBuffer = new BufferQueue(12, shmemName, err);
    if (err != "") spdlog::error(err.c_str());
    this->config = config;
}

SerialPort::~SerialPort() { delete m_bqBuffer; }
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
    if(m_iEpollFd != SFD_UNAVAILABLE)
        close(m_iEpollFd);
    if(m_pidChildProcess)
        kill(*m_pidChildProcess, SIGTERM);
    munmap(m_pidChildProcess, sizeof(*m_pidChildProcess));
}

//###################################################################################################
// Serial Port Initzialization. (Constructor)
//###################################################################################################

SerialPort::SerialPortImpl::SerialPortImpl(SerialPort* base, PortUtils::Serial::BaudRate baudRate): m_spBase(base) {
    std::string err = "";
    m_pidChildProcess = shalloc<pid_t>("child", err);
    if(err != "") spdlog::error(err.c_str());
}

SerialPort::SerialPortImpl::SerialPortImpl(SerialPort* base, PortUtils::Serial::PortConfig config): m_spBase(base) {
    std::string err = "";
    m_pidChildProcess = shalloc<pid_t>("child", err);
    if(err != "") spdlog::error(err.c_str());
}

//###################################################################################################
// Serial Port Cleanup. (Destructor)
//###################################################################################################

SerialPort::SerialPortImpl::~SerialPortImpl(){ clean(); }

//###################################################################################################
// Clean serial buffer.
//###################################################################################################

void SerialPort::SerialPortImpl::flush(){
    LINFO("Prepend");
}

//###################################################################################################
// Open a serial port.
//###################################################################################################

int SerialPort::SerialPortImpl::connect() {
    LINFO("Prepend");
    if((m_iFd = open(m_spBase->comPort.c_str(), O_RDWR | O_NOCTTY | O_NDELAY)) < 0){
        spdlog::error("pid({}) Error opening serial port: {}", getpid(), m_spBase->comPort.c_str());
        return -1;
    }

    // Reference: https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/
    // Create new termios struct, we call it 'tty' for convention
    // No need for "= {0}" at the end as we'll immediately write the existing
    // config to this struct.
    struct termios tty;

    // Read in existing settings, and handle any error
    // NOTE: This is important! POSIX states that the struct passed to tcsetattr()
    // must have been initialized with a call to tcgetattr() overwise behaviour
    // is undefined.
    if(tcgetattr(m_iFd, &tty) != 0){
         spdlog::error("pid({}) From tcgetattr: {}", getpid(), strerror(errno));
         clean();
         return -1;
    }

    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    tty.c_cc[VTIME] = 1;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    // Set in/out baud rate to be 9600
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    // Save tty settings, also checking for error
    if (tcsetattr(m_iFd, TCSANOW, &tty) != 0) {
        spdlog::error("pid({}) From tcsetattr: {}", getpid(), strerror(errno));
        clean();
        return -1;
    }

    sleep(2);
    
    if(fork() == 0) {
        *m_pidChildProcess = getpid();
        spdlog::debug("pid({}) Created child process.", m_pidChildProcess);

        struct epoll_event event;
        struct epoll_event* events;
        int length = 0;
        
        m_iEpollFd = epoll_create1(0);
        event.data.fd = m_iFd;
        event.events = EPOLLIN | EPOLLET;

        epoll_ctl(m_iEpollFd, EPOLL_CTL_ADD, m_iFd, &event);
        events = (struct epoll_event*)calloc(MAX_EVENTS, sizeof(event));

        int n;
        struct DataBlob* buffer = new DataBlob;
        for(;;) {
            if((n = epoll_wait(m_iEpollFd, events, MAX_EVENTS, 5000)) > 0) {
                if((length = ::read(events[0].data.fd, buffer->m_carrRawData, sizeof(buffer->m_carrRawData))) > 0) {
                    m_spBase->m_bqBuffer->write(*buffer);
                    spdlog::info("epoll: buffer: {}\n", buffer->m_carrRawData);
                } else spdlog::info("No data within 5 seconds. \n");
            }
        }

        free(events);
        clean();
    }
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

std::vector<std::string> SerialPort::getAvailablePorts() {
    return PortUtils::getAvailablePortsName(PortUtils::PortType::SERIAL, "/dev");
}


#endif // __linux__.
