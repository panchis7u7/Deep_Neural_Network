#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <string>
#include <vector>
#include <include/SerialPortWin.hpp>

constexpr auto EX_FATAL = 1; 

SerialPortWin::SerialPortWin(const wchar_t* comPort) {
	this->comPort = comPort;
	initPort(DEFAULT_COM_RATE, 8, COMMTIMEOUTS());
}

SerialPortWin::SerialPortWin(const wchar_t* comPort, SerialPortConf& serialConf) {
	this->comPort = comPort;
	initPort(serialConf.nComBits, serialConf.nComRate, serialConf.timeout);
}

SerialPortWin::~SerialPortWin() {

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Includes the functions for serial communication via RS232.
// Initializes the port and sets the communication parameters.
/////////////////////////////////////////////////////////////////////////////////////////////////////

void SerialPortWin::initPort(int nComRate, int nComBits, COMMTIMEOUTS timeout) {
	createPortFile();								// Initializes hCom to point to PORT#
	purgePort();									// Purges the COM port
	SetComParms(nComRate, nComBits, timeout);		// Uses the DCB structure to set up the COM port
	purgePort();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

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

void SerialPortWin::outputToPort(LPCVOID buf, DWORD szBuf) {
	int i=0;
	DWORD NumberofBytesTransmitted;
	LPDWORD lpErrors=0;
	LPCOMSTAT lpStat=0; 

	i = WriteFile(
		this->hCom,									// Write handle pointing to COM port
		buf,										// Buffer size
		szBuf,										// Size of buffer
		&NumberofBytesTransmitted,					// Written number of bytes
		NULL
	);
	// Handle the timeout error
	if (i == 0) {
		printf("\nWrite Error: 0x%x\n", GetLastError());
		ClearCommError(hCom, lpErrors, lpStat);		// Clears the device error flag to enable additional input and output operations. Retrieves information ofthe communications error.	
	}
	else
		printf("\nSuccessful transmission, there were %ld bytes transmitted\n", NumberofBytesTransmitted);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD SerialPortWin::inputFromPort(LPVOID buf, DWORD szBuf) {
	int i = 0;
	DWORD NumberofBytesRead;
	LPDWORD lpErrors = 0;
	LPCOMSTAT lpStat = 0;

	i = ReadFile(
		this->hCom,										// Read handle pointing to COM port
		buf,										// Buffer size
		szBuf,  									// Size of buffer - Maximum number of bytes to read
		&NumberofBytesRead,
		NULL
	);
	// Handle the timeout error
	if (i == 0) {
		printf("\nRead Error: 0x%x\n", GetLastError());
		ClearCommError(this->hCom, lpErrors, lpStat);		// Clears the device error flag to enable additional input and output operations. Retrieves information ofthe communications error.
	}
	else
		printf("\nSuccessful reception!, There were %ld bytes read\n", NumberofBytesRead);

	return(NumberofBytesRead);
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
		printf("\nFatal Error 0x%x: Unable to open\n", GetLastError());
	}
	else {
		printf("\nCOM is now open\n");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
int SerialPortWin::SetComParms(int nComRate, int nComBits, COMMTIMEOUTS timeout) {
	DCB dcb;										// Windows device control block
	// Clear DCB to start out clean, then get current settings
	memset(&dcb, 0, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	if (!GetCommState(this->hCom, &dcb))
		return(0);

	// Set our own parameters from Globals
	dcb.BaudRate = nComRate;						// Baud (bit) rate
	dcb.ByteSize = (BYTE)nComBits;					// Number of bits(8)
	dcb.Parity = 0;									// No parity	
	dcb.StopBits = ONESTOPBIT;						// One stop bit
	if (!SetCommState(this->hCom, &dcb))
		return(0);

	// Set communication timeouts (SEE COMMTIMEOUTS structure in MSDN) - want a fairly long timeout
	memset((void *)&timeout, 0, sizeof(timeout));
	timeout.ReadIntervalTimeout = 500;				// Maximum time allowed to elapse before arival of next byte in milliseconds. If the interval between the arrival of any two bytes exceeds this amount, the ReadFile operation is completed and buffered data is returned
	timeout.ReadTotalTimeoutMultiplier = 1;			// The multiplier used to calculate the total time-out period for read operations in milliseconds. For each read operation this value is multiplied by the requested number of bytes to be read
	timeout.ReadTotalTimeoutConstant = 5000;		// A constant added to the calculation of the total time-out period. This constant is added to the resulting product of the ReadTotalTimeoutMultiplier and the number of bytes (above).
	SetCommTimeouts(this->hCom, &timeout);
	return(1);
}

//#endif //Win32 implementation.
/////////////////////////////////////////////////////////////////////////////////////////////////////