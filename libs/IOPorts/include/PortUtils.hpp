#pragma once
#include "../platform.hpp"
#include <string>
#include <vector>
#include <filesystem>
#include <cstring>
#include <include/Logger.hpp>

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Port utility namespace.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

namespace PortUtils {

	LIBEXP typedef enum PortType {
		SERIAL
	} PortType;

	////////////////////////////////////////////////
	#ifdef WINDOWS_PLATFORM //WINDOWS_PLATFORM
	#include <windows.h>

	constexpr std::string_view PortTypeName[] = {
    	[PortType::SERIAL] = "COM";
	};
	LIBEXP struct Utils {
		static std::string* UTF16ToUTF8(const std::wstring& s) {
			const int size = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, NULL, 0, 0, NULL);
			std::vector<char> buf(size);
			WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, &buf[0], size, 0, NULL);
			return new std::string(&buf[0]);
		}
	};
	////////////////////////////////////////////////
	#elif defined(LINUX_PLATFORM) // LINUX_PLATFORM.

	constexpr std::string_view PortTypeName[] = {
    	[PortType::SERIAL] = "ttyUSB"
	};

	////////////////////////////////////////////////
	#elif defined(APPLE_PLATFORM) // APPLE_PLATFORM	

	constexpr std::string_view PortTypeName[] = {
    	[PortType::SERIAL] = "cu",
	};

	#endif

	LIBEXP static std::vector<std::string> getAvailablePortsName(PortType type, std::string deviceDirPath) {
	    std::vector<std::string> ports;
	    for(const auto& entry : std::filesystem::directory_iterator(deviceDirPath)) {
	        if(entry.path().string().find(PortTypeName[PortType::SERIAL]) != std::string::npos) {
	            ports.push_back(entry.path());
	            LINFO("%s", entry.path().string().c_str());
	        }
	    }
	    return ports;
	}

	// Generate an appropriate name for the shared memory id based of the com port name.
	LIBEXP static std::string shMemPortNameParser(std::string name, std::string delimiter) {
		std::vector<char*> vec;
    	char* token = strtok(const_cast<char*>(name.c_str()), delimiter.c_str());
    	while (token != nullptr) {
    	    vec.push_back(token);
    	    token = strtok(nullptr, "/");
    	}
		return std::string(vec.back()).insert(0, "/");
	}

	namespace Serial {
		LIBEXP typedef enum State {
			SS_Unknown,
			SS_UnInit,
			SS_Init,
			SS_Started,
			SS_Stopped
		} SerialState;

		LIBEXP typedef enum BaudRate {
    		B50 = 50,
    		B75 = 75,
    		B110 = 110,
    		B134 = 134,
    		B150 = 150,
    		B200 = 200,
    		B300 = 300,
    		B600 = 600,
    		B1200 = 1200,
    		B1800 = 1800,
    		B2400 = 2400,
    		B4800 = 4800,
    		B9600 = 9600,
    		B19200 = 19200,
    		B38400 = 38400
		} SerialBaudRate;

		LIBEXP constexpr SerialBaudRate DEFAULT_COM_RATE = BaudRate::B9600;
		LIBEXP constexpr int DEFAULT_COM_BITS = 8;
		LIBEXP constexpr int DEFAULT_PARITY_BITS = 1;

		LIBEXP typedef struct PortConfig {
			SerialBaudRate nComRate = DEFAULT_COM_RATE;
			unsigned nComBits = DEFAULT_COM_BITS;
			unsigned byStopBits;
			unsigned parity = DEFAULT_PARITY_BITS;
		} SerialPortConfig;
	};
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// End port utility namespace.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@