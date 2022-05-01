//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Serial Port MacOS Implementation class.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#include "../platform.hpp"

#ifdef APPLE_PLATFORM
// Custom library import.
#include <cstring>
#include <iostream>
#include <vector>
#include <include/Logger.hpp>
#include <include/SerialPort.hpp>   
#include <include/BufferQueue.hpp>
#include <include/SharedMessage.hpp>
#include <include/PortUtils.hpp>
// Serial configuration.
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <termios.h>// Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close(), fork()
#include <fcntl.h>  // Contains file controls like O_RDWR
#include <errno.h>  // Error integer and strerror() function
#include <signal.h>
// kqueue includes.
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/mman.h>

class SerialPort::SerialPortImpl {
public:
    SerialPortImpl(SerialPort* base, PortUtils::Serial::BaudRate baudRate = PortUtils::Serial::DEFAULT_COM_RATE);
    SerialPortImpl(SerialPort* base, PortUtils::Serial::PortConfig serialConfig);
    ~SerialPortImpl();

    void clean(pid_t process);
    void clean();
    void flush();
    int connect();
    std::string read();
    std::size_t write(void *data, std::size_t data_len);
    SerialPort* getBase() { return m_spBase; }
    
private:
    SerialPort* m_spBase;
    static constexpr int SFD_UNAVAILABLE = -1;
    int m_iFd = SFD_UNAVAILABLE;
    int m_iKq = SFD_UNAVAILABLE;
};

//###################################################################################################
// Platform independent abstraction definitions.
//###################################################################################################

SerialPort::SerialPort(std::string comPort, PortUtils::Serial::BaudRate baudRate) : comPort(comPort), m_pimpl(std::make_unique<SerialPortImpl>(this, baudRate)) {
    std::string err = "";
    std::string bufferName = PortUtils::shMemPortNameParser(comPort, "/");
    m_bqBuffer = new BufferQueue(12, bufferName, err);
    if (err != "") LERROR(err.c_str());
    config = PortUtils::Serial::PortConfig();
    config.nComRate = baudRate;
}

SerialPort::SerialPort(std::string comPort, PortUtils::Serial::PortConfig config): comPort(comPort), m_pimpl(std::make_unique<SerialPortImpl>(this, config)) {
    std::string err = "";
    std::string bufferName = PortUtils::shMemPortNameParser(comPort, "/");
    m_bqBuffer = new BufferQueue(12, bufferName, err);
    if (err != "") LERROR(err.c_str());
    this->config = config;
}

SerialPort::~SerialPort(){ if(m_bqBuffer){ delete m_bqBuffer;}}
void SerialPort::flush(){ pimpl()->flush(); }
int SerialPort::connect(){ return pimpl()->connect(); }
std::size_t SerialPort::write(void *data, std::size_t data_len) { return pimpl()->write(data, data_len); }
std::string SerialPort::read() { return pimpl()->read(); }

//###################################################################################################
// MacOS platform implementation.
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

SerialPort::SerialPortImpl::SerialPortImpl(SerialPort* base, PortUtils::Serial::BaudRate baudRate): m_spBase(base) {}
SerialPort::SerialPortImpl::SerialPortImpl(SerialPort* base, PortUtils::Serial::PortConfig serialConfig): m_spBase(base) {}

//###################################################################################################
// Serial Port Cleanup. (Destructor)
//###################################################################################################

SerialPort::SerialPortImpl::~SerialPortImpl(){
    if(m_iFd != SFD_UNAVAILABLE) {
        close(m_iFd);
    }
}

//###################################################################################################
// Clean serial buffer.
//###################################################################################################

void SerialPort::SerialPortImpl::flush() {
    ioctl(m_iFd, TCIOFLUSH, 2);
}

//###################################################################################################
// Open a serial port.
//###################################################################################################

int SerialPort::SerialPortImpl::connect() {
    if((m_iFd = open(m_spBase->comPort.c_str(), O_RDWR | O_NOCTTY | O_NDELAY)) < 0){
        LERROR("pid(%d) Error opening serial port: %s", getpid(), m_spBase->comPort.c_str());
        clean();
        return(-1);
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
    if(tcgetattr(m_iFd, &tty) != 0) {
        LERROR("pid(%d) From tcgetattr: %s", getpid(), strerror(errno));
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
        LERROR("pid(%d) From tcsetattr: %s", getpid(), strerror(errno));
        clean();
        return -1;
    }

    sleep(2);

    // Spawn process for serial buffer check.
    if(fork() == 0){
        pid_t childId = getpid();
        LDEBUG("pid(%d) Created child process.", childId);

        int kq;                 // kqueue file descriptor.
        struct kevent m_kRead;  // Event we want to monitor.
        struct kevent m_kEvent; // Event triggered.

        // Create a new kernel event queue;
        if((kq = kqueue()) < 0) {
            //std::cerr << "Error: " << errno << " from kqueue: " << strerror(errno) << "\n";
            LERROR("pid(%d) Error %d from kqueue: %s", errno, strerror(errno));
            clean(childId);
        }

        // Initialize kevent structure.
        EV_SET(&m_kRead, m_iFd, EVFILT_READ, EV_ADD, 0, 0, NULL);

        // Attach event to the kqueue.
        int ret = kevent(kq, &m_kRead, 1, NULL, 0, NULL);
        if (ret < 0){
            LERROR("pid(%d) Kevent failed: %s", strerror(errno));
            clean(childId);
            return -1;
        } else if (m_kRead.flags & EV_ERROR){
	        LERROR("pid(%d) Kevent failed: %s", strerror(errno));
            clean(childId);
            return -1;
        }

        struct timespec sleep_time;
        sleep_time.tv_sec = 1;
        sleep_time.tv_nsec = 0;

        for (;;) {
	        /*	Sleep until something happens. */
	        ret = kevent(kq, NULL, 0, &m_kEvent, 1, &sleep_time);
	        if	(ret == -1) {
                LERROR("pid(%d) Kevent wait: %s", strerror(errno));
		        clean(childId);
                return -1;
	        } else if (ret > 0) {
                LINFO("pid(%d) Recieved data on Serial Device: %s", m_spBase->comPort.c_str());
                std::cout << read() << "\n";
	        }
	    }
    }
    return 0;
}

//###################################################################################################
// Read serial buffer data if available.
//###################################################################################################

std::string SerialPort::SerialPortImpl::read()
{
    /*int bytes;
    ioctl(m_iFd, FIONREAD, &bytes);
    char buf[255] = {0};
    if(bytes > 0)
        read(m_iFd, buf, 255);
    std::cout << buf << "\n";
    return std::string(buf);*/
    
    //char buf[255] = {0};
    struct DataBlob* buffer = new DataBlob();
    if(::read(m_iFd, buffer->m_carrRawData, sizeof(buffer->m_carrRawData)) < 0) {
        LERROR("pid(%d) Error reading from device: %s", getpid(), strerror(errno));
        return "";
    }
    m_spBase->m_bqBuffer->write(*buffer);
    LDEBUG("kevent: %s", buffer->m_carrRawData);
    return std::string((char*)buffer->m_carrRawData);
}

//###################################################################################################
// Write to serial buffer if available.
//###################################################################################################

std::size_t SerialPort::SerialPortImpl::write(void *data, std::size_t data_len)
{
    if (::write(m_iFd, (char*)data, data_len) < 0) {
        LERROR("pid(%d) Error writing to device: %s", getpid(), strerror(errno));
        return 0;
    }
    return data_len;
}

//###################################################################################################
// Get available serial ports on the machine.
//###################################################################################################

std::vector<std::string> SerialPort::getAvailablePorts() {
    return PortUtils::getAvailablePortsName(PortUtils::PortType::SERIAL, "/dev");
}

#endif // __APPLE__