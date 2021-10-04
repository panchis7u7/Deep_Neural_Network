#pragma once

#include <list>

// Windows corresponding code.
#ifdef _WIN32
#include <Windows.h>

void initPort(HANDLE* hCom, wchar_t* COMPORT, int nComRate, int nComBits, COMMTIMEOUTS timeout);
void purgePort(HANDLE* hCom);
void outputToPort(HANDLE* hCom, LPCVOID buf, DWORD szBuf);
DWORD inputFromPort(HANDLE* hCom, LPVOID buf, DWORD szBuf);
std::list<int>* getAvailablePorts();

//Sub-Functions
void createPortFile(HANDLE* hCom, wchar_t* COMPORT);
static int SetComParms(HANDLE* hCom, int nComRate, int nComBits, COMMTIMEOUTS timeout);
static std::string utf16ToUTF8(const std::wstring& s);
#endif