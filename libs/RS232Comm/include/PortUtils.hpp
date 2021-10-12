#pragma once
#include <string>
#include <vector>
#include <Windows.h>


namespace PortUtils {
	struct Utils {
		static std::string* UTF16ToUTF8(const std::wstring& s) {
			const int size = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, NULL, 0, 0, NULL);
			std::vector<char> buf(size);
			WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, &buf[0], size, 0, NULL);
			return new std::string(&buf[0]);
		}
	};

	namespace Serial {
		typedef struct SerialPortConfig {
			unsigned nComRate;
			unsigned nComBits;
			unsigned byStopBits;
			unsigned parity;
		}SerialPortConfig;

		typedef enum SerialState {
			SS_Unknown,
			SS_UnInit,
			SS_Init,
			SS_Started,
			SS_Stopped,
		}SerialState;

		constexpr int DEFAULT_COM_RATE = 9600;
		constexpr int DEFAULT_COM_BITS = 8;
		constexpr int DEFAULT_PARITY_BITS = 1;
	};
}