//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Serial Port MacOS Implementation class.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#ifdef __APPLE__
#include <include/SerialPort.hpp>
#include <filesystem>
#include <iostream>
#include <vector>
// Serial configuration.
#include <termios.h> // Contains POSIX terminal control definitions
#include <fcntl.h> // Contains file controls like O_RDWR
#include <unistd.h> // write(), read(), close()
#include <errno.h> // Error integer and strerror() function
#include <sys/ioctl.h>
#include <sys/syscall.h>
// kqueue includes.
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/mman.h>
// Application quit.
#include <signal.h>

class SerialPort::SerialPortImpl
{
public:
    SerialPortImpl(SerialPort* base);
    ~SerialPortImpl();

    void clean(pid_t process);
    void clean();
    void flush();
    std::string readIfAvailable();
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

SerialPort::SerialPort(std::string com_port) : com_port(com_port), m_pimpl(std::make_unique<SerialPortImpl>(this)){}
SerialPort::~SerialPort(){}
void SerialPort::flush(){ pimpl()->flush(); }
std::size_t SerialPort::write(void *data, std::size_t data_len) { return pimpl()->write(data, data_len); }
std::string SerialPort::readIfAvailable() { return pimpl()->readIfAvailable(); }

//###################################################################################################
// MacOS platform implementation.
//###################################################################################################

void SerialPort::SerialPortImpl::clean(){
    if(m_iFd != SFD_UNAVAILABLE)
        close(m_iFd);
    exit(0);
}

void SerialPort::SerialPortImpl::clean(pid_t process){
    kill(process, SIGKILL);
    clean();
}

//###################################################################################################
// Serial Port Initzialization. (Constructor)
//###################################################################################################

SerialPort::SerialPortImpl::SerialPortImpl(SerialPort* base){
    m_spBase = base;
    if((m_iFd = open(base->com_port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY)) < 0){
        std::cerr << "Error opening serial port: " << base->com_port << "\n";
        clean();
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
        std::cerr << "Error: " << errno << " from tcgetattr: " << strerror(errno) << "\n";
        clean();
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
        std::cerr << "Error: " << errno << " from tcgetattr: " << strerror(errno) << "\n";
        clean();
    }

    sleep(2);

    pid_t process;
    if((process = fork()) == 0){

        int kq; // kqueue file descriptor.
        struct kevent m_kRead;  // Event we want to monitor.
        struct kevent m_kEvent; // Event triggered.

        // Create a new kernel event queue;
        if((kq = kqueue()) < 0) {
            std::cerr << "Error: " << errno << " from kqueue: " << strerror(errno) << "\n";
            clean();
        }

        // Initialize kevent structure.
        EV_SET(&m_kRead, m_iFd, EVFILT_READ, EV_ADD, 0, 0, NULL);

        // Attach event to the kqueue.
        int ret = kevent(kq, &m_kRead, 1, NULL, 0, NULL);
        if (ret < 0){
            std::cerr << "Kevent failed: " << strerror(errno) << "\n";
            clean();
        } else if (m_kRead.flags & EV_ERROR){
	        std::cerr << "Kevent error: " << strerror(errno) << "\n";
            clean();
        }

        struct timespec sleep_time;
        sleep_time.tv_sec = 1;
        sleep_time.tv_nsec = 0;

        for (;;) {
	        /*	Sleep until something happens. */
	        ret = kevent(kq, NULL, 0, &m_kEvent, 1, &sleep_time);
	        if	(ret == -1) {
                std::cerr << "Kevent wait: " << strerror(errno) << "\n";
		        clean(process);
	        } else if (ret > 0) {
		        std::cout << "Recieved data on serial device: " << base->com_port << "\n";
                std::cout << readIfAvailable() << "\n";
	        }
	    }
    }
}

//###################################################################################################
// Serial Port Cleanup. (Destructor)
//###################################################################################################

SerialPort::SerialPortImpl::~SerialPortImpl(){
    if(m_iFd != SFD_UNAVAILABLE) {
        close(m_iFd);
    }
}

void SerialPort::SerialPortImpl::flush() {

}

std::string SerialPort::SerialPortImpl::readIfAvailable()
{
    /*int bytes;
    ioctl(m_iFd, FIONREAD, &bytes);
    char buf[255] = {0};
    if(bytes > 0)
        read(m_iFd, buf, 255);
    std::cout << buf << "\n";
    return std::string(buf);*/
    
    char buf[255] = {0};
    read(m_iFd, buf, 255);
    return std::string(buf);
}

std::size_t SerialPort::SerialPortImpl::write(void *data, std::size_t data_len)
{
    if (::write(m_iFd, (char*)data, data_len) < 0) {
        std::cerr << "Error writing to device! ( " << data << ") "<< "\n";
    }
    return data_len;
}

std::vector<std::string> SerialPort::getAvailablePorts()
{
    std::string path = "/dev";
    std::vector<std::string> ports;
    for(const auto& entry : std::filesystem::directory_iterator(path)) {
        if(entry.path().string().find("cu") != std::string::npos) {
            ports.push_back(entry.path());
            std::cout << entry.path() << std::endl;
        }
    }
    return ports;
}

#endif // __APPLE__