#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <process.h>
#include <include/SerialPortWin.hpp>
#include <include/PortUtils.hpp>

constexpr auto EX_FATAL = 1; 
using namespace PortUtils::Serial;

//###################################################################################################
// 1st Constructor.
//###################################################################################################

SerialPortWin::SerialPortWin(const wchar_t* comPort) {
	this->m_wComPort = comPort;
	this->m_wSerialConf = {PortUtils::Serial::DEFAULT_COM_RATE, PortUtils::Serial::DEFAULT_COM_BITS, ONESTOPBIT, 0, COMMTIMEOUTS() };
	InvalidateHandle(m_hThread);
	InvalidateHandle(m_hThreadStarted);
	InvalidateHandle(m_hThreadTerm);
	InvalidateHandle(m_hCom);
	InvalidateHandle(m_hDataRx);
	InitPort();
}

//###################################################################################################
// 2nd Constructor.
//###################################################################################################

SerialPortWin::SerialPortWin(const wchar_t* comPort, WinSerialPortConf& serialConf) {
	this->m_wComPort = comPort;
	this->m_wSerialConf = serialConf;
	InvalidateHandle(m_hThread);
	InvalidateHandle(m_hThreadStarted);
	InvalidateHandle(m_hThreadTerm);
	InvalidateHandle(m_hCom);
	InvalidateHandle(m_hDataRx);
	InitPort();
}

//###################################################################################################
// Destructor.
//###################################################################################################

SerialPortWin::~SerialPortWin() {
	PurgePort();
	CloseHandle(this->m_hCom);
}

//###################################################################################################
// Includes the functions for serial communication via RS232.
// Initializes the port and sets the communication parameters.
//###################################################################################################

HRESULT SerialPortWin::InitPort() {
	CreatePortFile();								// Initializes hCom to point to PORT#
	//purgePort();									// Purges the COM port
	SetComParms();									// Uses the DCB structure to set up the COM port
	SetupEvent();
	//purgePort();
	//std::thread* eventThread = new std::thread(&SerialPortWin::eventThreadFn, this);
	return S_OK;
}

//###################################################################################################
// Handle uninitializer.
//###################################################################################################

void SerialPortWin::InvalidateHandle(HANDLE& hHandle) {
	hHandle = INVALID_HANDLE_VALUE;
}

//###################################################################################################
// Close and Clean Handle.
//###################################################################################################

void SerialPortWin::CloseAndCleanHandle(HANDLE& hHandle) {
	BOOL abRet = CloseHandle(hHandle);
	if (!abRet) {
		assert(0);
	}
	InvalidateHandle(hHandle);
}

//###################################################################################################
// Create port file.
//###################################################################################################
// Set the hCom HANDLE to point to a COM port, initialize for reading and writing, 
// open the port and set securities.
//###################################################################################################

HRESULT SerialPortWin::CreatePortFile() {
	HRESULT hr = S_OK;

	m_hDataRx = CreateEvent(0, 0, 0, 0);
	m_hCom = CreateFile(
		PortUtils::Utils::UTF16ToUTF8(this->m_wComPort)->c_str(),	// COM port number  --> If COM# is larger than 9 then use the following syntax--> "\\\\.\\COM10"
		GENERIC_READ | GENERIC_WRITE,							// Open for read and write
		NULL,													// No sharing allowed
		NULL,													// No security
		OPEN_EXISTING,											// Opens the existing com port
		FILE_FLAG_OVERLAPPED,									// Do not set any file attributes --> Use synchronous operation
		NULL													// No template
	);

	SetupComm(m_hCom, 512, 512);
	
	if (m_hCom == INVALID_HANDLE_VALUE) {
		std::cout << "Fatal Error" << GetLastError() << ": Unable to open." << std::endl;
		return E_FAIL;
	} else {
		std::cout << PortUtils::Utils::UTF16ToUTF8(this->m_wComPort)->c_str() << " is now open." << std::endl;
	}
	return hr;
}

//###################################################################################################
// Purge any outstanding requests on the serial port (initialize)
//###################################################################################################

HRESULT SerialPortWin::PurgePort() {
	PurgeComm(m_hCom, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
	return S_OK;
}

//###################################################################################################
// Set Serial COM port configuration parameters.
//###################################################################################################

HRESULT SerialPortWin::SetComParms() {
	if (!SetCommMask(m_hCom, EV_RXCHAR | EV_TXEMPTY)) {
		std::cout << "RS232Win: Failed to Get Comm Mask. Reason: " << GetLastError() << std::endl;
		return E_FAIL;
	}

	// Windows device control block.
	DCB dcb = {0};
	// Clear DCB to start out clean, then get current settings
	dcb.DCBlength = sizeof(dcb);
	//memset(&dcb, 0, sizeof(dcb));

	if (!GetCommState(m_hCom, &dcb)) {
		std::cout << "RS232Win: Failed to Get Comm State Reason: " << GetLastError() << std::endl;
		return E_FAIL;
	}

	// Set our own parameters from Globals
	dcb.BaudRate = m_wSerialConf.nComRate;			// Baud (bit) rate
	dcb.ByteSize = (BYTE)m_wSerialConf.nComBits;	// Number of bits(8)
	dcb.Parity = m_wSerialConf.parity;				// No parity (0)
	dcb.fDsrSensitivity = 0;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fOutxDsrFlow = 0;

	if (m_wSerialConf.byStopBits == 1)
		dcb.StopBits = ONESTOPBIT;
	else if (m_wSerialConf.byStopBits == 2)
		dcb.StopBits = TWOSTOPBITS;
	else
		dcb.StopBits = ONE5STOPBITS;

	if (!SetCommState(m_hCom, &dcb)) {
		assert(0);
		std::cout << "RS232Win: : Failed to Set Comm State Reason: " << GetLastError() << std::endl;
		return E_FAIL;
	}

	// Set communication timeouts (SEE COMMTIMEOUTS structure in MSDN) - want a fairly long timeout
	//COMMTIMEOUTS* timeout = &this->m_wSerialConf.timeout;
	//memset((void*)timeout, 0, sizeof(*timeout));
	//timeout->ReadIntervalTimeout = 500;				// Maximum time allowed to elapse before arival of next byte in milliseconds. If the interval between the arrival of any two bytes exceeds this amount, the ReadFile operation is completed and buffered data is returned
	//timeout->ReadTotalTimeoutMultiplier = 1;		// The multiplier used to calculate the total time-out period for read operations in milliseconds. For each read operation this value is multiplied by the requested number of bytes to be read
	//timeout->ReadTotalTimeoutConstant = 5000;		// A constant added to the calculation of the total time-out period. This constant is added to the resulting product of the ReadTotalTimeoutMultiplier and the number of bytes (above).
	//SetCommTimeouts(this->m_hCom, timeout);

	COMMTIMEOUTS* timeout = &this->m_wSerialConf.timeout;
	timeout->ReadIntervalTimeout = MAXDWORD;
	timeout->ReadTotalTimeoutMultiplier = 0;
	timeout->ReadTotalTimeoutConstant = 0;
	timeout->WriteTotalTimeoutMultiplier = 0;
	timeout->WriteTotalTimeoutConstant = 0;
	
	if (!SetCommTimeouts(m_hCom, timeout)) {
		assert(0);
		std::cout << "RS232Win: : Error setting timeouts, Reason: " << GetLastError() << std::endl;
		return E_FAIL;
	}

	std::cout << "RS232Win: Current Settings, (Baud Rate= " << dcb.BaudRate << "; Parity= " << (int)dcb.Parity <<
		"; Byte Size= " << (int)dcb.ByteSize << "; Stop Bits= " << (int)dcb.StopBits << ";)" << std::endl;
	return(S_OK);
}

//###################################################################################################
// Setup event for reading cycle.
//###################################################################################################

HRESULT SerialPortWin::SetupEvent() {
	m_hThreadTerm = CreateEvent(0,0,0,0);
	m_hThreadStarted = CreateEvent(0,0,0,0);
	m_hThread = (HANDLE)_beginthreadex(0, 0, SerialPortWin::eventThreadFn, (void*)this, 0, 0);
	
	DWORD dwWait = WaitForSingleObject(m_hThreadStarted, INFINITE);
	assert(dwWait == WAIT_OBJECT_0);
	CloseHandle(m_hThreadStarted);
	InvalidateHandle(m_hThreadStarted);
	m_abIsConnected = true;

	return 0;
}

//###################################################################################################
// Thread function for non polling.
//###################################################################################################

unsigned int __stdcall SerialPortWin::eventThreadFn(void* pvParam) {
	SerialPortWin* apThis = (SerialPortWin*)pvParam;
	bool abContinue = true;
	DWORD dwEventMask = 0;
	HANDLE arHandles[2] = {nullptr};
	DWORD dwWait;

	OVERLAPPED ov = {0};
	ov.hEvent = CreateEvent(
		NULL,   // default security attributes 
		TRUE,   // manual-reset event 
		FALSE,  // not signaled 
		NULL    // no name
	);

	arHandles[0] = apThis->m_hThreadTerm;
	SetEvent(apThis->m_hThreadStarted);

	while (abContinue) {
		BOOL abRet = WaitCommEvent(apThis->m_hCom, &dwEventMask, &ov);
		if (!abRet) {
			assert(GetLastError() == ERROR_IO_PENDING);
		}

		arHandles[1] = ov.hEvent;

		dwWait = WaitForMultipleObjects(2, arHandles, FALSE, INFINITE);
		switch (dwWait) {
			case WAIT_OBJECT_0: {
				_endthreadex(1);
			}
			break;
			case WAIT_OBJECT_0 + 1: {
				DWORD dwMask;
				if (GetCommMask(apThis->m_hCom, &dwMask)) {
					if (dwMask == EV_TXEMPTY) {
						std::cout << "Data sent." << std::endl;
						ResetEvent(ov.hEvent);
						continue;
					}
				}

				//Data Read.
				int iAccum = 0;
				apThis->m_serialBuffer.LockBuffer();
				try {
					BOOL abRet = false;
					DWORD dwBytesRead = 0;
					OVERLAPPED ovRead;
					memset(&ovRead, 0, sizeof(ovRead));
					ovRead.hEvent = CreateEvent(0, true, 0, 0);

					do {
						ResetEvent(ovRead.hEvent);
						char szTemp[1];
						int iSize = sizeof(szTemp);
						memset(&szTemp, 0, sizeof(szTemp));
						abRet = ReadFile(apThis->m_hCom, (LPVOID)szTemp, sizeof(szTemp), &dwBytesRead, &ovRead);
						//std::cout << szTemp[0];
						if (!abRet) {
							abContinue = FALSE;
							break;
						}
						if (dwBytesRead > 0) {
							apThis->m_serialBuffer.addData(szTemp, dwBytesRead);
							iAccum += dwBytesRead;
						}
					} while (dwBytesRead > 0);
					CloseHandle(ovRead.hEvent);
				}
				catch (...) {
					assert(0);
				}
				//if we are not in started state then we should flush the queue...( we would still read the data)
				if (apThis->GetCurrentState() != SS_Started)
				{
					iAccum = 0;
					apThis->m_serialBuffer.flush();
				}

				apThis->m_serialBuffer.UnLockBuffer();

				if (iAccum > 0)
					apThis->SetDataReadEvent();

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

std::size_t SerialPortWin::Write(void* data, std::size_t data_len) {
	OVERLAPPED osWrite = { 0 };
	DWORD dwRes = 0;
	DWORD dwBytesTransmitted;
	BOOL fRes;

	Sleep(500);
	// Create this write operation's OVERLAPPED structure's hEvent.
	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osWrite.hEvent == NULL) 
		// error creating overlapped event handle
		return FALSE;

	//Issue write operation.
	if (!WriteFile(m_hCom, (LPCVOID)data, (DWORD)data_len, &dwBytesTransmitted, &osWrite)) {
		if (GetLastError() != ERROR_IO_PENDING) {
			// WriteFile failed, but isn't delayed. Report error and abort.
			std::cout << "\nWrite Error: " << GetLastError() << std::endl;
			fRes = FALSE;
		} else {
			// Write is pending.
			if (!GetOverlappedResult(m_hCom, &osWrite, &dwBytesTransmitted, TRUE))
				fRes = FALSE;
			else {
				// Write operation completed successfully.
				fRes = TRUE;
				std::cout << "Successful transmission, there were " << dwBytesTransmitted << " bytes transmitted." << std::endl;
			}
		}
	} else {
		// WriteFile completed immediately.
		fRes = TRUE;
	}

	CloseHandle(osWrite.hEvent);
	return dwBytesTransmitted;
}

//###################################################################################################
// Read data from COM port.
//###################################################################################################

std::size_t SerialPortWin::Read(void* buf, std::size_t buf_len) {
	DWORD dwRead;
	BOOL fWaitingOnRead = FALSE;
	OVERLAPPED osReader = { 0 };

	// Create the overlapped event. Must be closed before exiting
	// to avoid a handle leak.
	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osReader.hEvent == NULL)
		// Error creating overlapped event; abort.
		return E_FAIL;

	if (!fWaitingOnRead) {
		// Issue read operation.
		if (!ReadFile(m_hCom, (LPVOID)buf, (DWORD)buf_len, &dwRead, &osReader)) {
			if (GetLastError() != ERROR_IO_PENDING)     // read not delayed?
				// Error in communications; report it.
				std::cout << "\nRead Error: " << GetLastError() << std::endl;
			else
				fWaitingOnRead = TRUE;
		} else {
			// read completed immediately
			std::cout << "Successful reception!, There were " << dwRead << " bytes read" << std::endl;
			//HandleASuccessfulRead(lpBuf, dwRead);
		}
	}

	CloseHandle(osReader.hEvent);
	return dwRead;
}

void SerialPortWin::Read(std::string& buf) {
	m_serialBuffer.GetDataIfAvailable();
}

//###################################################################################################
// List com ports available on the device.
//###################################################################################################

std::vector<std::wstring> SerialPortWin::GetAvailablePorts() {
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

HRESULT SerialPortWin::CanProcess() {
	switch (m_eState) {
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

//###################################################################################################
// -- Reads all the data that is available in the local buffer.. 
// does NOT make any blocking calls in case the local buffer is empty
//###################################################################################################

/*HRESULT SerialPortWin::ReadAvailable(std::string& data) {
	HRESULT hr = CanProcess();
	if (FAILED(hr)) return hr;
	try {
		bool abRet = m_serialBuffer.GetDataIfAvailable().size() > 0;
		ResetEvent(m_hDataRx);
	}
	catch (...) {
		hr = E_FAIL;
	}
	return hr;
}*/