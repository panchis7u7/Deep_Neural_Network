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
	this->m_wComPort = comPort;
	this->m_wSerialConf = {DEFAULT_COM_RATE, 8, ONESTOPBIT, 0, COMMTIMEOUTS() };
	initPort();
}

SerialPortWin::SerialPortWin(const wchar_t* comPort, WinSerialPortConf& serialConf) {
	this->m_wComPort = comPort;
	this->m_wSerialConf = serialConf;
	initPort();
}

SerialPortWin::~SerialPortWin() {
	purgePort();
	CloseHandle(this->m_hCom);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Includes the functions for serial communication via RS232.
// Initializes the port and sets the communication parameters.
/////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT SerialPortWin::initPort() {
	createPortFile();								// Initializes hCom to point to PORT#
	purgePort();									// Purges the COM port
	setComParms();									// Uses the DCB structure to set up the COM port
	setupEvent();
	purgePort();
	//std::thread* eventThread = new std::thread(&SerialPortWin::eventThreadFn, this);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Sub functions called by above functions
/**************************************************************************************/
// Set the hCom HANDLE to point to a COM port, initialize for reading and writing, open the port and set securities
HRESULT SerialPortWin::createPortFile() {
	// Call the CreateFile() function 
	HRESULT hr = S_OK;

	this->m_hDataRx = CreateEvent(0, 0, 0, 0);
	this->m_hCom = CreateFile(
		SerialPortWin::utf16ToUTF8(this->m_wComPort)->c_str(),	// COM port number  --> If COM# is larger than 9 then use the following syntax--> "\\\\.\\COM10"
		GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE,			// Open for read and write
		NULL,													// No sharing allowed
		NULL,													// No security
		OPEN_EXISTING,											// Opens the existing com port
		FILE_ATTRIBUTE_NORMAL,									// Do not set any file attributes --> Use synchronous operation
		NULL													// No template
	);

	if (this->m_hCom == INVALID_HANDLE_VALUE) {
		std::cout << "Fatal Error" << GetLastError() << ": Unable to open." << std::endl;
		return E_FAIL;
	}
	else {
		std::cout << AbstractPort::utf16ToUTF8(this->m_wComPort)->c_str() << " is now open." << std::endl;
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Purge any outstanding requests on the serial port (initialize)
/////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT SerialPortWin::purgePort() {
	PurgeComm(this->m_hCom, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT SerialPortWin::setComParms() {
	if (!SetCommMask(this->m_hCom, EV_RXCHAR | EV_TXEMPTY)) {
		std::cout << "RS232Win: Failed to Get Comm Mask. Reason: " << GetLastError() << std::endl;
		return E_FAIL;
	}

	// Windows device control block.
	DCB dcb = {0};
	// Clear DCB to start out clean, then get current settings
	dcb.DCBlength = sizeof(dcb);
	//memset(&dcb, 0, sizeof(dcb));

	if (!GetCommState(this->m_hCom, &dcb)) {
		std::cout << "RS232Win: Failed to Get Comm State Reason: " << GetLastError() << std::endl;
		return E_FAIL;
	}

	// Set our own parameters from Globals
	dcb.BaudRate = this->m_wSerialConf.nComRate;		// Baud (bit) rate
	dcb.ByteSize = (BYTE)this->m_wSerialConf.nComBits;	// Number of bits(8)
	dcb.Parity = this->m_wSerialConf.parity;			// No parity (0)

	/*if (this->m_wSerialConf.byStopBits == 1)
		dcb.StopBits = ONESTOPBIT;
	else if (this->m_wSerialConf.byStopBits == 2)
		dcb.StopBits = TWOSTOPBITS;
	else
		dcb.StopBits = ONE5STOPBITS;*/

	if (!SetCommState(this->m_hCom, &dcb)) {
		assert(0);
		std::cout << "RS232Win: : Failed to Set Comm State Reason: " << GetLastError() << std::endl;
		return E_FAIL;
	}

	// Set communication timeouts (SEE COMMTIMEOUTS structure in MSDN) - want a fairly long timeout
	COMMTIMEOUTS* timeout = &this->m_wSerialConf.timeout;
	memset((void*)timeout, 0, sizeof(*timeout));
	timeout->ReadIntervalTimeout = 500;				// Maximum time allowed to elapse before arival of next byte in milliseconds. If the interval between the arrival of any two bytes exceeds this amount, the ReadFile operation is completed and buffered data is returned
	timeout->ReadTotalTimeoutMultiplier = 1;		// The multiplier used to calculate the total time-out period for read operations in milliseconds. For each read operation this value is multiplied by the requested number of bytes to be read
	timeout->ReadTotalTimeoutConstant = 5000;		// A constant added to the calculation of the total time-out period. This constant is added to the resulting product of the ReadTotalTimeoutMultiplier and the number of bytes (above).
	SetCommTimeouts(this->m_hCom, timeout);

	std::cout << "RS232Win: Current Settings, (Baud Rate= " << dcb.BaudRate << "; Parity= " << dcb.Parity <<
		"Byte Size= " << dcb.ByteSize << "; Stop Bits= " << dcb.StopBits << "." << std::endl;
	return(S_OK);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT SerialPortWin::setupEvent() {
	this->m_hThreadTerm = CreateEvent(0,0,0,0);
	this->m_hThreadStarted = CreateEvent(0,0,0,0);
	this->m_hThread = (HANDLE)_beginthreadex(0, 0, SerialPortWin::eventThreadFn, (void*)this, 0, 0);
	return 0;
}

unsigned int __stdcall SerialPortWin::eventThreadFn(void* pvParam) {
	SerialPortWin* apThis = (SerialPortWin*)pvParam;
	bool abContinue = true;
	DWORD dwEventMask = 0;
	HANDLE arHandles[2];
	DWORD dwWait;

	OVERLAPPED o = {0};
	o.hEvent = CreateEvent(
		NULL,   // default security attributes 
		TRUE,   // manual-reset event 
		FALSE,  // not signaled 
		NULL    // no name
	);

	arHandles[0] = apThis->m_hThreadTerm;
	SetEvent(apThis->m_hThreadStarted);

	while (abContinue) {
		BOOL abRet = WaitCommEvent(apThis->m_hCom, &dwEventMask, &o);
		if (!abRet) {
			assert(GetLastError() == ERROR_IO_PENDING);
		}

		arHandles[1] = o.hEvent;

		dwWait = WaitForMultipleObjects(2, arHandles, FALSE, INFINITE);
		switch (dwWait) {
			case WAIT_OBJECT_0:
				{
					_endthreadex(1);
				}
				break;
			case WAIT_OBJECT_0 + 1:
				{
					DWORD dwMask;
					if (GetCommMask(apThis->m_hCom, &dwMask)) {
						if (dwMask == EV_TXEMPTY) {
							std::cout << "Data sent." << std::endl;
							ResetEvent(o.hEvent);
							continue;
						}
					}

					//Data Read.
					int iAccum = 0;
					apThis->m_serialBuffer.
				}
				break;
		}
	}

	return 0;

	/*BOOL fSuccess = SetCommMask(this->m_hCom, EV_CTS | EV_DSR);
	if (!fSuccess) {
		printf("SetCommMask failed with error %d.\n", GetLastError());
		return;
	}

	assert(o.hEvent);
	DWORD dwEvtMask = 0;

	if (WaitCommEvent(m_hCom, &dwEvtMask, &o))
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
	}*/
}

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
		this->m_hCom,								// Write handle pointing to COM port
		buf,										// Buffer size
		buf_len,									// Size of buffer
		&nBytesTransmited,							// Written number of bytes
		NULL
	);
	// Handle the timeout error
	if (i == 0) {
		printf("\nWrite Error: 0x%x\n", GetLastError());
		ClearCommError(this->m_hCom, lpErrors, lpStat);		// Clears the device error flag to enable additional input and output operations. Retrieves information ofthe communications error.	
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
		this->m_hCom,								// Read handle pointing to COM port
		buf,										// Buffer size
		buf_len,  									// Size of buffer - Maximum number of bytes to read
		&nBytesRead,
		NULL
	);
	// Handle the timeout error
	if (i == 0) {
		printf("\nRead Error: 0x%x\n", GetLastError());
		ClearCommError(this->m_hCom, lpErrors, lpStat);		// Clears the device error flag to enable additional input and output operations. Retrieves information ofthe communications error.
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