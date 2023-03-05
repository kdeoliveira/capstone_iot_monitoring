#pragma once

#include "common.hpp"


bool awaitService(const char* type, SC_HANDLE& schService, SERVICE_STATUS_PROCESS& status, DWORD statusAwaited);
bool QueryStatus(SC_HANDLE& schService, SERVICE_STATUS_PROCESS& status, LPDWORD dwBytes);


void sleep(const char* str, int ms) {
	for (int i = 0; i < ms/100; i++) {
		Sleep(100);
		std::cout << str;
	}
}

/// <summary>
/// Install this application as a windows service
/// </summary>
bool serviceInstallation() {
	SC_HANDLE schManager;
	SC_HANDLE schService;
	
	char module_path_name[MAX_SIZE];
	if (!::GetModuleFileNameA(nullptr, module_path_name, MAX_SIZE)) {
		return false;
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
		std::cout << "Unable to open Service Manager\n" << GetFormattedSystemError() << std::endl;
		return false;
	}


	
	schService = ::CreateServiceA(
		schManager,
		SERVICE_NAME,
		SERVICE_NAME,
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
		std::cout << "Unable to open Service Manager\n" << GetFormattedSystemError() << std::endl;

		return false;
	}

	// Sets the description for this service
	SERVICE_DESCRIPTIONA desc = { "IoT monitoring endpoint for communicating with LoRa-based devices" };
	if (!::ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &desc)) {
		std::cout << "Unable to set service description\n" << GetFormattedSystemError() << std::endl;

	}

	CloseServiceHandle(schManager);
	CloseServiceHandle(schService);

	std::cout << SERVICE_NAME << " has successfully been installed" << std::endl;

	return true;
}

/// <summary>
/// Attempts the start the service 
/// </summary>
void startService(int argc, char** argv) {
	SERVICE_STATUS_PROCESS status;
	

	SC_HANDLE schServiceManager = OpenSCManager(
		NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS
	);

	if (schServiceManager == nullptr) {
		std::cout << "Unable to open Service Manager\n" << GetFormattedSystemError() << std::endl;
		return;
	}

	SC_HANDLE schService = OpenService(
		schServiceManager,
		SERVICE_NAME,
		SERVICE_ALL_ACCESS
	);

	if (schServiceManager == nullptr) {
		CloseServiceHandle(schServiceManager);
		std::cout << "Unable to open Service Handler\n" << GetFormattedSystemError() << std::endl;
		return;
	}

	DWORD dwBytes;
	if (!QueryStatus(
		schService,
		status,
		&dwBytes
	)) {
		CloseServiceHandle(schServiceManager);
		return;
	}
	
	if (status.dwCurrentState != SERVICE_STOPPED && status.dwCurrentState != SERVICE_STOP_PENDING) {
		std::cout << "Service currently running" << std::endl;
		CloseServiceHandle(schServiceManager);
		CloseServiceHandle(schService);
		return;
	}

	if (!awaitService("Stopping",schService, status, SERVICE_STOP_PENDING)) {
		CloseServiceHandle(schServiceManager);
		return;
	}

	if (!StartService(schService, argc, (LPCSTR*)argv)) {
		CloseServiceHandle(schServiceManager);
		CloseServiceHandle(schService);
		std::cout << "Unable to start the service\n" << GetFormattedSystemError() << std::endl;

		return;
	}

	if (!QueryStatus(
		schService,
		status,
		&dwBytes
	)) {
		CloseServiceHandle(schServiceManager);
		return;
	}

	if (!awaitService("Starting", schService, status, SERVICE_START_PENDING)) {
		CloseServiceHandle(schServiceManager);
		return;
	}

	if (status.dwCurrentState == SERVICE_RUNNING) {
		std::cout << SERVICE_NAME << " has successfully started" << std::endl;
	}
	else {
		std::cout << SERVICE_NAME << " has not started" << std::endl;
	}
	
}

bool QueryStatus(SC_HANDLE& schService, SERVICE_STATUS_PROCESS& status, LPDWORD dwBytes) {
	if (!QueryServiceStatusEx(
		schService,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&status,
		sizeof(SERVICE_STATUS_PROCESS),
		dwBytes
	)) {
		CloseServiceHandle(schService);
		std::cout << "Unable to query service\n" << GetFormattedSystemError() << std::endl;

		return false;
	}
	else {
		return true;
	}
}

/// <summary>
/// Awaits the service to change to a different status for the given WaitHint time 
/// </summary>
bool awaitService(const char* type, SC_HANDLE& schService, SERVICE_STATUS_PROCESS& status, DWORD statusAwaited) {
	if (status.dwCurrentState != statusAwaited) return true;
	DWORD startCount = GetTickCount();
	DWORD checkpointCount = status.dwCheckPoint;
	DWORD waitTime;
	DWORD numBytes;

	std::cout << type << " ";
	while (status.dwCurrentState == statusAwaited) {
		waitTime = status.dwWaitHint / 10;

		if (waitTime < 1000) waitTime = 1000;
		else if (waitTime > 10000) waitTime = 10000;

		sleep(".", waitTime);

		if (!QueryStatus(
			schService,
			status,
			&numBytes
		)) {
			return false;
		}

		//CheckPoint has been incremented; Service is still trying to stop/start
		if (status.dwCheckPoint > checkpointCount) {
			startCount = GetTickCount();
			checkpointCount = status.dwCheckPoint;
		}
		else {
			//checkpoint is set to 0 when stopped or run
			if (GetTickCount() - startCount > status.dwWaitHint && status.dwCheckPoint != 0) {
				CloseServiceHandle(schService);
				std::cout << "Timeout awaiter\n";
				return false;
			}
		}
	}
	std::cout << std::endl;
	return true;
}



