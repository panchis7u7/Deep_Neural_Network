//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Serial Port MacOS Implementation class.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#ifdef __APPLE__
#include <include/SerialPort.hpp>
#include <filesystem>
#include <iostream>
#include <vector>
#include <termios.h> // Contains POSIX terminal control definitions
#include <fcntl.h> // Contains file controls like O_RDWR
#include <unistd.h> // write(), read(), close()
#include <errno.h> // Error integer and strerror() function
#include <sys/ioctl.h>

class SerialPort::SerialPortImpl
{
public:
    SerialPortImpl(SerialPort* base);
    ~SerialPortImpl();

    void flush();
    std::string readIfAvailable();
    std::size_t write(void *data, std::size_t data_len);
    SerialPort* getBase() { return this->base; }
    
private:
    SerialPort* base;
    static constexpr int SFD_UNAVAILABLE = -1;
    int fd = SFD_UNAVAILABLE;
};

SerialPort::SerialPort(std::string com_port) : com_port(com_port), m_pimpl(std::make_unique<SerialPortImpl>(this)){}

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
    return pimpl()->readIfAvailable();
}

SerialPort::SerialPortImpl::SerialPortImpl(SerialPort* base){
    this->base = base;
    if((fd = open(base->com_port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY)) < 0){
        std::cerr << "Error opening serial port: " << base->com_port << "\n";
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
    if(tcgetattr(fd, &tty) != 0) {
        std::cerr << "Error: " << errno << " from tcgetattr: " << strerror(errno) << "\n";
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
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::cerr << "Error: " << errno << " from tcgetattr: " << strerror(errno) << "\n";
    }

    sleep(2);
}

SerialPort::SerialPortImpl::~SerialPortImpl(){
    if(fd != SFD_UNAVAILABLE) {
        close(fd);
    }
}

void SerialPort::SerialPortImpl::flush() {

}

std::string SerialPort::SerialPortImpl::readIfAvailable()
{
    int bytes;
    ioctl(fd, FIONREAD, &bytes);
    char buf[255] = {0};
    if(bytes > 0)
        read(fd, buf, 255);
    std::cout << buf << "\n";
    return std::string(buf);
}

std::size_t SerialPort::SerialPortImpl::write(void *data, std::size_t data_len)
{
    if (::write(fd, (char*)data, data_len) < 0) {
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