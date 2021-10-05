#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <string>
#include <vector>
#include <assert.h>
#include <thread>
#include <include/SerialPortWin.hpp>

constexpr auto EX_FATAL = 1; 

SerialPortWin::SerialPortWin(const wchar_t* comPort) {
	this->comPort = comPort;
	this->serialConf = {DEFAULT_COM_RATE, 8, COMMTIMEOUTS()};
	initPort();
}

SerialPortWin::SerialPortWin(const wchar_t* comPort, WinSerialPortConf& serialConf) {
	this->comPort = comPort;
	this->serialConf = serialConf;
	initPort();
}

SerialPortWin::~SerialPortWin() {
	purgePort();
	CloseHandle(this->hCom);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Includes the functions for serial communication via RS232.
// Initializes the port and sets the communication parameters.
/////////////////////////////////////////////////////////////////////////////////////////////////////

void SerialPortWin::initPort() {
	createPortFile();								// Initializes hCom to point to PORT#
	purgePort();									// Purges the COM port
	SetComParms();									// Uses the DCB structure to set up the COM port
	purgePort();
	std::thread* eventThread = new std::thread(&SerialPortWin::setupEvent, this);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////


void SerialPortWin::setupEvent() {
	BOOL fSuccess = SetCommMask(this->hCom, EV_CTS | EV_DSR);
	if (!fSuccess) {
		printf("SetCommMask failed with error %d.\n", GetLastError());
		return;
	}

	OVERLAPPED o;
	o.hEvent = CreateEvent(
		NULL,   // default security attributes 
		TRUE,   // manual-reset event 
		FALSE,  // not signaled 
		NULL    // no name
	);

	// Initialize the rest of the OVERLAPPED structure to zero.
	o.Internal = 0;
	o.InternalHigh = 0;
	o.Offset = 0;
	o.OffsetHigh = 0;

	assert(o.hEvent);
	DWORD dwEvtMask = 0;

	if (WaitCommEvent(hCom, &dwEvtMask, &o))
	{
		//Data set ready.
		if (dwEvtMask & EV_DSR)
		{
			std::cout << "Hola" << std::endl;
		}

		//Clear to send.
		if (dwEvtMask & EV_CTS)
		{
			std::cout << "Hola" << std::endl;
		}
	}
	else
	{
		DWORD dwRet = GetLastError();
		if (ERROR_IO_PENDING == dwRet)
		{
			printf("I/O is pending...\n");

			// To do.
		}
		else
			printf("Wait failed with error %d.\n", GetLastError());
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
// Purge any outstanding requests on the serial port (initialize)
/////////////////////////////////////////////////////////////////////////////////////////////////////

void SerialPortWin::purgePort() {
	PurgeComm(this->hCom, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Output/Input messages to/from ports 
/////////////////////////////////////////////////////////////////////////////////////////////////////

// LPCVOID buf, DWORD szBuf
std::size_t SerialPortWin::sendData(void* buf, std::size_t buf_len) {
	int i = 0;
	DWORD nBytesTransmited;
	LPDWORD lpErrors = 0;
	LPCOMSTAT lpStat = 0;

	i = WriteFile(
		this->hCom,									// Write handle pointing to COM port
		buf,										// Buffer size
		buf_len,									// Size of buffer
		&nBytesTransmited,							// Written number of bytes
		NULL
	);
	// Handle the timeout error
	if (i == 0) {
		printf("\nWrite Error: 0x%x\n", GetLastError());
		ClearCommError(this->hCom, lpErrors, lpStat);		// Clears the device error flag to enable additional input and output operations. Retrieves information ofthe communications error.	
	}
	else
		std::cout << "Successful transmission, there were " << nBytesTransmited << " bytes transmitted." << std::endl;
	return nBytesTransmited;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////

// LPVOID buf, DWORD szBuf
std::size_t SerialPortWin::rcvData(void* buf, std::size_t buf_len) {
	int i = 0;
	DWORD nBytesRead;
	LPDWORD lpErrors = 0;
	LPCOMSTAT lpStat = 0;

	i = ReadFile(
		this->hCom,									// Read handle pointing to COM port
		buf,										// Buffer size
		buf_len,  									// Size of buffer - Maximum number of bytes to read
		&nBytesRead,
		NULL
	);
	// Handle the timeout error
	if (i == 0) {
		printf("\nRead Error: 0x%x\n", GetLastError());
		ClearCommError(this->hCom, lpErrors, lpStat);		// Clears the device error flag to enable additional input and output operations. Retrieves information ofthe communications error.
	}
	else
		std::cout << "Successful reception!, There were " << nBytesRead << " bytes read" << std::endl;

	return(nBytesRead);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
// List com ports available on the device.
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::wstring> SerialPortWin::getAvailablePorts() {
	//Buffer to store the path of the COM PORTS
	LPWSTR lpTargetPath = (LPWSTR)calloc(5000, sizeof(wchar_t));
	std::vector<std::wstring> portList;

	//Checking ports from COM0 to COM255
	for (int i = 0; i < 255; i++) 
	{
		//Converting to COM0, COM1, COM2, ... COMN.
		std::wstring str = L"COM" + std::to_wstring(i);
		DWORD res = QueryDosDeviceW((LPCWSTR)str.c_str(), lpTargetPath, 5000);
		// Test the return value and error if any
		if (res != 0) //QueryDosDevice returns zero if it didn't find an object
		{
			portList.push_back(str);
			//std::wcout << str << ": " << lpTargetPath << std::endl;
		}
		if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			free(lpTargetPath);
		}
	}

	free(lpTargetPath);
	return portList;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Sub functions called by above functions
/**************************************************************************************/
// Set the hCom HANDLE to point to a COM port, initialize for reading and writing, open the port and set securities
void SerialPortWin::createPortFile() {
	// Call the CreateFile() function 
	this->hCom = CreateFile(
	 	SerialPortWin::utf16ToUTF8(this->comPort)->c_str(),		// COM port number  --> If COM# is larger than 9 then use the following syntax--> "\\\\.\\COM10"
		GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE,			// Open for read and write
		NULL,													// No sharing allowed
		NULL,													// No security
		OPEN_EXISTING,											// Opens the existing com port
		FILE_ATTRIBUTE_NORMAL,									// Do not set any file attributes --> Use synchronous operation
		NULL													// No template
	);
	
	if (this->hCom == INVALID_HANDLE_VALUE) {
		std::cout << "Fatal Error" << GetLastError() << ": Unable to open." << std::endl;
	}
	else {
		std::cout << AbstractSerialPort::utf16ToUTF8(this->comPort)->c_str() << " is now open." << std::endl;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
int SerialPortWin::SetComParms() {
	DCB dcb;										// Windows device control block
	// Clear DCB to start out clean, then get current settings
	memset(&dcb, 0, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	if (!GetCommState(this->hCom, &dcb))
		return(0);

	// Set our own parameters from Globals
	dcb.BaudRate = this->serialConf.nComRate;		// Baud (bit) rate
	dcb.ByteSize = (BYTE)this->serialConf.nComBits;	// Number of bits(8)
	dcb.Parity = 0;									// No parity	
	dcb.StopBits = ONESTOPBIT;						// One stop bit
	if (!SetCommState(this->hCom, &dcb))
		return(0);

	// Set communication timeouts (SEE COMMTIMEOUTS structure in MSDN) - want a fairly long timeout
	COMMTIMEOUTS* timeout = &this->serialConf.timeout;
	memset((void *)timeout, 0, sizeof(*timeout));
	timeout->ReadIntervalTimeout = 500;				// Maximum time allowed to elapse before arival of next byte in milliseconds. If the interval between the arrival of any two bytes exceeds this amount, the ReadFile operation is completed and buffered data is returned
	timeout->ReadTotalTimeoutMultiplier = 1;			// The multiplier used to calculate the total time-out period for read operations in milliseconds. For each read operation this value is multiplied by the requested number of bytes to be read
	timeout->ReadTotalTimeoutConstant = 5000;		// A constant added to the calculation of the total time-out period. This constant is added to the resulting product of the ReadTotalTimeoutMultiplier and the number of bytes (above).
	SetCommTimeouts(this->hCom, timeout);
	return(1);
}

//#endif //Win32 implementation.
/////////////////////////////////////////////////////////////////////////////////////////////////////