//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Serial Port Windows Implementation class.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#if defined(WIN32) || defined(_WIN64) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <AbstractPort.hpp>
#include <SerialBuffer.hpp>
#include <PortUtils.hpp>
#include <SerialPort.hpp>
#include <iostream>
#include <assert.h>
#include <process.h>
#include <Windows.h>

using namespace PortUtils::Serial;

typedef struct WinSerialPortConfig : SerialPortConfig
{
    COMMTIMEOUTS timeout;
} WinSerialPortConf;

class SerialPort::SerialPortImpl
{
public:
    SerialPortImpl(std::string com_port);
    SerialPortImpl(std::string com_port, WinSerialPortConf &serialConf);
    ~SerialPortImpl();

    static unsigned int __stdcall eventThreadFn(void *pvParam);

    HRESULT createPortFile();
    HRESULT setComParms();
    HRESULT setupEvent();
    HRESULT canProcess();
    HRESULT inline purge();

    void invalidateHandle(HANDLE &hHandle);
    void closeAndCleanHandle(HANDLE &hHandle);
    inline void setDataReadEvent() { SetEvent(m_hDataRx); }
    inline SerialState getCurrentState() { return m_eState; }
    std::size_t read(void *buf, std::size_t buf_len);

    // Polymorphic functions.
    void flush();
    std::size_t write(void *data, std::size_t data_len);
    std::string readIfAvailable();

private:
    std::string m_sComPort;
    bool m_aIsReading;
    bool m_abIsConnected;
    HANDLE m_hCom;
    HANDLE m_hThreadTerm;
    HANDLE m_hThread;
    HANDLE m_hThreadStarted;
    HANDLE m_hDataRx;
    SerialState m_eState;
    SerialBuffer m_serialBuffer;
    WinSerialPortConf m_wSerialConf;
};

//###################################################################################################
// Windows platform implementation.
//###################################################################################################

SerialPort::SerialPort(std::string com_port) : com_port(com_port), m_pimpl(std::make_unique<SerialPortImpl>(com_port)) {}
SerialPort::~SerialPort() {}
void SerialPort::flush() { pimpl()->flush(); }
std::size_t SerialPort::write(void* data, std::size_t data_len) { return pimpl()->write(data, data_len); }
std::string SerialPort::readIfAvailable() { return pimpl()->readIfAvailable(); }

//###################################################################################################
// Purge any outstanding requests on the serial port (initialize)
//###################################################################################################

HRESULT inline SerialPort::SerialPortImpl::purge(){
    PurgeComm(m_hCom, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
    return S_OK;
}

//###################################################################################################
// Includes the functions for serial communication via RS232.
// Initializes the port and sets the communication parameters.
//###################################################################################################

SerialPort::SerialPortImpl::SerialPortImpl(std::string com_port)
{
    this->m_sComPort = com_port;
    this->m_wSerialConf = {DEFAULT_COM_RATE, DEFAULT_COM_BITS, ONESTOPBIT, 0, COMMTIMEOUTS()};
    invalidateHandle(m_hThread);
    invalidateHandle(m_hThreadStarted);
    invalidateHandle(m_hThreadTerm);
    invalidateHandle(m_hCom);
    invalidateHandle(m_hDataRx);
    // Initializes hCom to point to COM PORT#.
    createPortFile();
    // Purges the COM port
    // purgePort();
    // Uses the DCB structure to set up the COM port.
    setComParms();
    // Set event handles for reading / writing.
    setupEvent();
}

SerialPort::SerialPortImpl::SerialPortImpl(std::string comPort, WinSerialPortConf &serialConf)
{
    this->m_sComPort = comPort;
    this->m_wSerialConf = serialConf;
    invalidateHandle(m_hThread);
    invalidateHandle(m_hThreadStarted);
    invalidateHandle(m_hThreadTerm);
    invalidateHandle(m_hCom);
    invalidateHandle(m_hDataRx);
    // Initializes hCom to point to COM PORT#.
    createPortFile();
    // Purges the COM port
    // purgePort();
    // Uses the DCB structure to set up the COM port.
    setComParms();
    // Set event handles for reading / writing.
    setupEvent();
}

SerialPort::SerialPortImpl::~SerialPortImpl()
{
    purge();
    CloseHandle(this->m_hCom);
}

//###################################################################################################
// Handle uninitializer.
//###################################################################################################

void SerialPort::SerialPortImpl::invalidateHandle(HANDLE &hHandle)
{
    hHandle = INVALID_HANDLE_VALUE;
}

//###################################################################################################
// Close and Clean Handle.
//###################################################################################################

void SerialPort::SerialPortImpl::closeAndCleanHandle(HANDLE &hHandle)
{
    BOOL abRet = CloseHandle(hHandle);
    if (!abRet)
    {
        assert(0);
    }
    invalidateHandle(hHandle);
}

//###################################################################################################
// Create port file.
//###################################################################################################
// Set the hCom HANDLE to point to a COM port, initialize for reading and writing,
// open the port and set securities.
//###################################################################################################

HRESULT SerialPort::SerialPortImpl::createPortFile()
{
    HRESULT hr = S_OK;

    m_hDataRx = CreateEvent(0, 0, 0, 0);
    m_hCom = CreateFile(
        // PortUtils::Utils::UTF16ToUTF8(this->m_wComPort)->c_str(), // COM port number  --> If COM# is larger than 9 then use the following syntax--> "\\\\.\\COM10"
        this->m_sComPort.c_str(),
        GENERIC_READ | GENERIC_WRITE, // Open for read and write
        NULL,                         // No sharing allowed
        NULL,                         // No security
        OPEN_EXISTING,                // Opens the existing com port
        FILE_FLAG_OVERLAPPED,         // Do not set any file attributes --> Use synchronous operation
        NULL                          // No template
    );

    // Set buffer size.
    SetupComm(m_hCom, 512, 512);

    if (m_hCom == INVALID_HANDLE_VALUE)
    {
        std::cout << "Fatal Error" << GetLastError() << ": Unable to open." << std::endl;
        return E_FAIL;
    }
    else
        std::cout << this->m_sComPort << " is now open!" << std::endl;
    // std::cout << PortUtils::Utils::UTF16ToUTF8(this->m_wComPort)->c_str() << " is now open." << std::endl
    return hr;
}

//###################################################################################################
// Set Serial COM port configuration parameters.
//###################################################################################################

HRESULT SerialPort::SerialPortImpl::setComParms()
{
    if (!SetCommMask(m_hCom, EV_RXCHAR | EV_TXEMPTY))
    {
        std::cout << "RS232Win: Failed to Get Comm Mask. Reason: " << GetLastError() << std::endl;
        return E_FAIL;
    }

    // Windows device control block.
    DCB dcb = {0};
    // Clear DCB to start out clean, then get current settings
    dcb.DCBlength = sizeof(dcb);
    // memset(&dcb, 0, sizeof(dcb));

    if (!GetCommState(m_hCom, &dcb))
    {
        std::cout << "RS232Win: Failed to Get Comm State Reason: " << GetLastError() << std::endl;
        return E_FAIL;
    }

    // Set our own parameters from Globals
    dcb.BaudRate = m_wSerialConf.nComRate;       // Baud (bit) rate
    dcb.ByteSize = (BYTE)m_wSerialConf.nComBits; // Number of bits(8)
    dcb.Parity = m_wSerialConf.parity;           // No parity (0)
    dcb.fDsrSensitivity = 0;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;
    dcb.fOutxDsrFlow = 0;

    if (m_wSerialConf.byStopBits == 1)
        dcb.StopBits = ONESTOPBIT;
    else if (m_wSerialConf.byStopBits == 2)
        dcb.StopBits = TWOSTOPBITS;
    else
        dcb.StopBits = ONE5STOPBITS;

    if (!SetCommState(m_hCom, &dcb))
    {
        assert(0);
        std::cout << "RS232Win: : Failed to Set Comm State Reason: " << GetLastError() << std::endl;
        return E_FAIL;
    }

    // Set communication timeouts (SEE COMMTIMEOUTS structure in MSDN) - want a fairly long timeout
    // COMMTIMEOUTS* timeout = &this->m_wSerialConf.timeout;
    // memset((void*)timeout, 0, sizeof(*timeout));
    // timeout->ReadIntervalTimeout = 500;				// Maximum time allowed to elapse before arival of next byte in milliseconds. If the interval between the arrival of any two bytes exceeds this amount, the ReadFile operation is completed and buffered data is returned
    // timeout->ReadTotalTimeoutMultiplier = 1;		// The multiplier used to calculate the total time-out period for read operations in milliseconds. For each read operation this value is multiplied by the requested number of bytes to be read
    // timeout->ReadTotalTimeoutConstant = 5000;		// A constant added to the calculation of the total time-out period. This constant is added to the resulting product of the ReadTotalTimeoutMultiplier and the number of bytes (above).
    // SetCommTimeouts(this->m_hCom, timeout);

    COMMTIMEOUTS *timeout = &this->m_wSerialConf.timeout;
    timeout->ReadIntervalTimeout = MAXDWORD;
    timeout->ReadTotalTimeoutMultiplier = 0;
    timeout->ReadTotalTimeoutConstant = 0;
    timeout->WriteTotalTimeoutMultiplier = 0;
    timeout->WriteTotalTimeoutConstant = 0;

    if (!SetCommTimeouts(m_hCom, timeout))
    {
        assert(0);
        std::cout << "RS232Win: : Error setting timeouts, Reason: " << GetLastError() << std::endl;
        return E_FAIL;
    }

    std::cout << "RS232Win: Current Settings, (Baud Rate= " << dcb.BaudRate << "; Parity= " << (int)dcb.Parity << "; Byte Size= " << (int)dcb.ByteSize << "; Stop Bits= " << (int)dcb.StopBits << ";)" << std::endl;
    return (S_OK);
}

//###################################################################################################
// Setup event for reading cycle.
//###################################################################################################

HRESULT SerialPort::SerialPortImpl::setupEvent()
{
    m_hThreadTerm = CreateEvent(0, 0, 0, 0);
    m_hThreadStarted = CreateEvent(0, 0, 0, 0);
    m_hThread = (HANDLE)_beginthreadex(0, 0, SerialPortImpl::eventThreadFn, (void *)this, 0, 0);

    DWORD dwWait = WaitForSingleObject(m_hThreadStarted, INFINITE);
    assert(dwWait == WAIT_OBJECT_0);
    CloseHandle(m_hThreadStarted);
    invalidateHandle(m_hThreadStarted);
    m_abIsConnected = true;

    return 0;
}

//###################################################################################################
// Thread function for non polling.
//###################################################################################################

unsigned int __stdcall SerialPort::SerialPortImpl::eventThreadFn(void *pvParam)
{
    SerialPortImpl *apThis = (SerialPortImpl *)pvParam;
    bool abContinue = true;
    DWORD dwEventMask = 0;
    HANDLE arHandles[2] = {nullptr};
    DWORD dwWait;

    OVERLAPPED ov = {0};
    ov.hEvent = CreateEvent(
        NULL,  // default security attributes
        TRUE,  // manual-reset event
        FALSE, // not signaled
        NULL   // no name
    );

    arHandles[0] = apThis->m_hThreadTerm;
    SetEvent(apThis->m_hThreadStarted);
    apThis->m_eState = SS_Started;

    while (abContinue)
    {
        BOOL abRet = WaitCommEvent(apThis->m_hCom, &dwEventMask, &ov);
        if (!abRet)
        {
            assert(GetLastError() == ERROR_IO_PENDING);
        }

        arHandles[1] = ov.hEvent;

        dwWait = WaitForMultipleObjects(2, arHandles, FALSE, INFINITE);
        switch (dwWait)
        {
        case WAIT_OBJECT_0:
        {
            _endthreadex(1);
        }
        break;
        case WAIT_OBJECT_0 + 1:
        {
            DWORD dwMask;
            if (GetCommMask(apThis->m_hCom, &dwMask))
            {
                if (dwMask == EV_TXEMPTY)
                {
                    std::cout << "Data sent." << std::endl;
                    ResetEvent(ov.hEvent);
                    continue;
                }
            }

            // Data Read.
            int iAccum = 0;
            apThis->m_serialBuffer.LockBuffer();
            try
            {
                BOOL abRet = false;
                DWORD dwBytesRead = 0;
                OVERLAPPED ovRead = {0};
                memset(&ovRead, 0, sizeof(ovRead));
                ovRead.hEvent = CreateEvent(0, true, 0, 0);

                do
                {
                    ResetEvent(ovRead.hEvent);
                    char szTemp[1];
                    int iSize = sizeof(szTemp);
                    memset(&szTemp, 0, sizeof(szTemp));
                    Sleep(1);
                    abRet = ReadFile(apThis->m_hCom, (LPVOID)szTemp, sizeof(szTemp), &dwBytesRead, &ovRead);
                    if (!abRet)
                    {
                        abContinue = FALSE;
                        break;
                    }
                    if (dwBytesRead > 0)
                    {
                        apThis->m_serialBuffer.addData(szTemp, dwBytesRead);
                        iAccum += dwBytesRead;
                    }
                } while (dwBytesRead > 0);
                CloseHandle(ovRead.hEvent);
            }
            catch (...)
            {
                assert(0);
            }
            // if we are not in started state then we should flush the queue...( we would still read the data)
            if (apThis->getCurrentState() != SS_Started)
            {
                iAccum = 0;
                apThis->m_serialBuffer.flush();
            }

            apThis->m_serialBuffer.UnLockBuffer();

            if (iAccum > 0)
                apThis->setDataReadEvent();

            ResetEvent(ov.hEvent);
        }
        break;
        }
    }

    return 0;
}

//###################################################################################################
// Write data to COM port.
//###################################################################################################

std::size_t SerialPort::SerialPortImpl::write(void *data, std::size_t data_len)
{
    OVERLAPPED osWrite = {0};
    DWORD dwRes = 0;
    DWORD dwBytesTransmitted;
    BOOL fRes;

    Sleep(1000);
    // Create this write operation's OVERLAPPED structure's hEvent.
    osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (osWrite.hEvent == NULL)
        // error creating overlapped event handle
        return FALSE;

    // Issue write operation.
    if (!WriteFile(m_hCom, (LPCVOID)data, (DWORD)data_len, &dwBytesTransmitted, &osWrite))
    {
        if (GetLastError() != ERROR_IO_PENDING)
        {
            // WriteFile failed, but isn't delayed. Report error and abort.
            std::cout << "\nWrite Error: " << GetLastError() << std::endl;
            fRes = FALSE;
        }
        else
        {
            // Write is pending.
            if (!GetOverlappedResult(m_hCom, &osWrite, &dwBytesTransmitted, TRUE))
                fRes = FALSE;
            else
            {
                // Write operation completed successfully.
                fRes = TRUE;
                std::cout << "Successful transmission, there were " << dwBytesTransmitted << " bytes transmitted." << std::endl;
            }
        }
    }
    else
    {
        // WriteFile completed immediately.
        fRes = TRUE;
    }

    CloseHandle(osWrite.hEvent);
    return dwBytesTransmitted;
}

//###################################################################################################
// Read data from COM port.
//###################################################################################################

std::size_t SerialPort::SerialPortImpl::read(void *buf, std::size_t buf_len)
{
    DWORD dwRead;
    BOOL fWaitingOnRead = FALSE;
    OVERLAPPED osReader = {0};

    // Create the overlapped event. Must be closed before exiting
    // to avoid a handle leak.
    osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (osReader.hEvent == NULL)
        // Error creating overlapped event; abort.
        return E_FAIL;

    if (!fWaitingOnRead)
    {
        // Issue read operation.
        if (!ReadFile(m_hCom, (LPVOID)buf, (DWORD)buf_len, &dwRead, &osReader))
        {
            if (GetLastError() != ERROR_IO_PENDING) // read not delayed?
                // Error in communications; report it.
                std::cout << "\nRead Error: " << GetLastError() << std::endl;
            else
                fWaitingOnRead = TRUE;
        }
        else
        {
            // read completed immediately
            std::cout << "Successful reception!, There were " << dwRead << " bytes read" << std::endl;
            // HandleASuccessfulRead(lpBuf, dwRead);
        }
    }

    CloseHandle(osReader.hEvent);
    return dwRead;
}

//###################################################################################################
// Clear serial buffers.
//###################################################################################################

void SerialPort::SerialPortImpl::flush() {

}

//###################################################################################################
// Read data from COM port if data is in the buffer, else, just wait.
//###################################################################################################

std::string SerialPort::SerialPortImpl::readIfAvailable()
{
    return m_serialBuffer.GetDataIfAvailable();
    ResetEvent(m_hDataRx);
}

//###################################################################################################
// List com ports available on the device.
//###################################################################################################

std::vector<std::string> SerialPort::getAvailablePorts()
{
    // Buffer to store the path of the COM PORTS
    LPWSTR lpTargetPath = (LPWSTR)calloc(5000, sizeof(wchar_t));
    std::vector<std::string> portList;

    // Checking ports from COM0 to COM255
    for (int i = 0; i < 255; i++)
    {
        // Converting to COM0, COM1, COM2, ... COMN.
        // std::wstring str = L"COM" + std::to_wstring(i);
        std::string str = "COM" + i;
        DWORD res = QueryDosDeviceW((LPCWSTR)str.c_str(), lpTargetPath, 5000);
        // Test the return value and error if any
        if (res != 0) // QueryDosDevice returns zero if it didn't find an object
        {
            portList.push_back(str);
            // std::wcout << str << ": " << lpTargetPath << std::endl;
        }
        if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            free(lpTargetPath);
        }
    }

    free(lpTargetPath);
    return portList;
}

HRESULT SerialPort::SerialPortImpl::canProcess()
{
    switch (m_eState)
    {
    case SS_Unknown:
        assert(0);
        return E_FAIL;
        break;
    case SS_UnInit:
        return E_FAIL;
        break;
    case SS_Started:
        return S_OK;
        break;
    case SS_Init:
        break;
    case SS_Stopped:
        return E_FAIL;
        break;
    default:
        assert(0);
        break;
    }
    return E_FAIL;
}

#endif // _WIN64.