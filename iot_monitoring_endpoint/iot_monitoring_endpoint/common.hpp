#pragma once

#include <windows.h>
#include <string>
#include <strsafe.h>

#include <exception>

#pragma comment(lib, "advapi32.lib")

#define MAX_SIZE 2048


static iot_monitoring::arg_handler h;

#define SERVICE_NAME "iot-monitoring"

//https://stackoverflow.com/questions/455434/how-should-i-use-formatmessage-properly-in-c
std::string GetFormattedSystemError() {
	DWORD err = GetLastError();

	LPTSTR buff = nullptr;

	std::string error;
	DWORD rt = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		err,
		LANG_SYSTEM_DEFAULT,
		(LPSTR)&buff,
		0,
		NULL
	);
	
	if (buff != NULL) {
		error =  std::string(buff);
		LocalFree(buff);
	}

	return error;
}