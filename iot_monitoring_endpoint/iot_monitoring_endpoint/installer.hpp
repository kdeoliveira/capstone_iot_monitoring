#pragma once

#include "common.hpp"

void serviceInstallation(int argc, char** argv) {
	SC_HANDLE schManager;
	SC_HANDLE schService;
	
	char module_path_name[MAX_SIZE];
	if (!::GetModuleFileNameA(nullptr, module_path_name, MAX_SIZE)) {
		throw std::exception("unable to find current path");
	}

	char moduleWithQuotes[MAX_SIZE];
	::StringCbPrintfA(moduleWithQuotes, MAX_SIZE, TEXT("\"%s\""), module_path_name);
	
	std::string moduleName(moduleWithQuotes);

	auto args = h.getArguments();

	std::for_each(args.begin(), args.end(), [&](std::string str) {
			if (str != "-interactive" && str != "-INTERACTIVE" && str != "-install" && str != "-INSTALL") {
				moduleName += " ";
				auto it = std::find(str.begin(), str.end(), (char)"\\");
				if (it != str.end()) {
					char escaped[MAX_SIZE];
					::StringCbPrintfA(escaped, MAX_SIZE, TEXT("\"%s\""), (char*) str.c_str());
					moduleName += escaped;
				}
				else {
					moduleName += str;
				}

			}
		});

	schManager = OpenSCManager(
		NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS
	);

	if (NULL == schManager)
	{
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return;
	}


	
	schService = ::CreateServiceA(
		schManager,
		"iot-monitoring",
		"iot-monitoring",
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START,
		SERVICE_ERROR_NORMAL,
		moduleName.c_str(),
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	);

	if (schService == nullptr) {
		CloseServiceHandle(schManager);
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return;
	}

	CloseServiceHandle(schManager);
	CloseServiceHandle(schService);
}